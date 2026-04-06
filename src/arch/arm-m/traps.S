/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <machine/assembler.h>
#include <arch/machine.h>
#include <arch/machine/registerset.h>
#include <util.h>

.syntax unified
.thumb

.extern kernel_stack_alloc
.extern ki_bss_start, ki_bss_end
.extern ki_relocation_regions, ki_relocation_regions_end
.extern ksCurThread

.section .boot.text, "ax"
BEGIN_FUNC(arm_Reset_exception)
    /* zero out BSS, we can't do this in C as we can't use the stack */
    ldr r0, =ki_bss_start  @ incremented during loop
    ldr r1, =ki_bss_end
    @ fill value
    movs r2, #0
_zero_bss_begin:
    cmp r0, r1
    @ exit out if r0 >= r1
    bhs _zero_bss_done
    @ zero out r0; word-sized stores; then r0 += 4
    str r2, [r0], #4
    b _zero_bss_begin
_zero_bss_done:

    /* relocate data from flash to sram */
    ldr r0, =ki_relocation_regions_start   @ incremented during loop
    ldr r1, =ki_relocation_regions_end
_relocate_regions_compare:
    cmp r0, r1
    @ exit out if r0 >= r1
    bhs _relocate_regions_done

    ldr r2, [r0], #4 @ r2: addr, incremented
    ldr r3, [r0], #4 @ r3: loadaddr, incremented
    ldr r4, [r0], #4 @ r4: size
    adds r4, r2, r4  @ r4: end addr
_relocate_reg_compare:
    cmp r2, r4
    @ exit if r2 (addr) >= r4 (end)
    bhs _relocate_reg_done

    @ copy from load to addr and increment
    ldr r5, [r3], #4
    str r5, [r2], #4

    b _relocate_reg_compare
_relocate_reg_done:
    b _relocate_regions_compare
_relocate_regions_done:

    /* Not all platforms will look at the vector table to set the stack */
    ldr sp, =(kernel_stack_alloc + BIT(CONFIG_KERNEL_STACK_BITS))

    bl init_kernel

    @ In order to begin execution of userspace, we want to transition to an
    @ unprivileged thread mode (difficult to do directly, as enabling interrupts
    @ or configuring other processor state (e.g. MPU or SP_SEL) and setting
    @ CONTROL_nPRIV are two different non-atomic operations). We can do this
    @ from 'Handler mode' via 'Exception Return', but now the problem becomes
    @ how to transition into Handler mode.
    @
    @ Arm-M does not provide explicit control over the current execution mode,
    @ per §E.2.1.81 'CurrentMode' of Armv8-M ARM DDI 0553B.y it is solely
    @ dependent on the current value of the IPSR.Exception field, which is
    @ only adjusted during exception entry/return: §E2.1.16 'ActiveException'.
    @
    @ One solution to this problem (taken e.g. by OxideComputer's Hubris [1]
    @ or by ARM in their 'Interrupt Deprivileging' [2] examples) is to perform
    @ an SVC at the end of kernel initialisation, which has either a runtime
    @ system call cost (in checking for this state) or needs faffing around with
    @ replacing entries of the vector table.
    @
    @ Another solution we attempted was ARM's 'context-switch-fp' demonstrator
    @ from the 'Armv8-M Architecture - User Guide Examples' GitHub repository
    @ (https://github.com/ARM-software/m-profile-user-guide-examples), which
    @ instead treats initialisation as part of the idle task, using the SysTick
    @ interrupt to *then* switch to the root task in Handler mode. However,
    @ this really has the same problem: turning on CONTROL_nPRIV races with
    @ the SysTick timer firing. It is, in fact, worse than the original
    @ emulation of exception return as now if an interrupt happens then
    @ CONTROL_nPRIV may never end up being set until the idle thread runs...
    @ which might be a *while* away, as the root task generally does a lot of
    @ initialisation.
    @
    @ In the end, what we've decided on is that some of the following code is
    @ notionally 'part' of the root task. Specifically, when we setup the
    @ registers we can do this as part of our trusted code loading off the
    @ process stack. At this point the MPU will have been set up by kernel -
    @ but because this is located in the .boot.text, which is part of the
    @ memory userspace is given access to, this is fine.

    @ Load the amount of data we need to get out of the kernel into registers
    ldr r0, =ksCurThread /* &NODE_STATE(ksCurThread) */
    ldr r0, [r0]
    @ offset to the kernel saved registers
    adds r0, #PT_R4
    @ load the kernel-saved registers (r4-r11, SP [into r12], but not EXC_RETURN)
    ldm r0, {r4-r11, r12}
    @ write SP_process to PSP
    msr PSP, r12
    isb

    @ switch to using SP_process now.
    mrs r0, CONTROL
    orr r0, #(CONTROL_SPSEL)
    msr CONTROL, r0
    isb

    @ Turn on interrupts. We are still in privileged thread mode, however,
    @ we are running on the process stack and no longer need to access anything
    @ from kernel data structures; so if we are interrupted into the kernel,
    @ nothing bad will happen. This is also because there is only one runnable
    @ thread at the moment: us. Any forward progress the system can make must
    @ be towards the setting of CONTROL_nPRIV next - it is not possible to enter
    @ state where other threads progress in privileged modes.
    cpsie i

    @ Switch to unprivileged mode now. From here on we are a "true" userspace
    @ task.
    mrs r0, CONTROL
    orr r0, #(CONTROL_nPRIV)
    msr CONTROL, r0
    isb

    @ pop the 'hardware-saved' registers off the stack.
    @ this replaces our temporary $r0 with the one from the register context;
    ldm sp, {r0-r3, r12, lr}
    @ increment the stack pointer to the top of the stack.
    @ we can't use ldm sp! because we can't pop PC and xPSR.
    @ framesize = 0x20
    add sp, #0x20

    @ jump to thread entrypoint by reading the pc off the stack.
    @ PT_R4 is the offset of the top of the hardware-stacked registers (which
    @ happens to be R4) and NextIP is the offset of the PC, so we move
    @ down the difference in position down from the top of the stack.
    ldr pc, [sp, #(PT_NextIP - PT_R4)]
END_FUNC(arm_Reset_exception)

/**
 * Each of the following exceptions behave following the ExceptionEntry()
 * psuedocode in §B1.5.6 Armv7-M ARM DDI 0403E.e.
 *
 * We have disabled the FPU, so framesize = 0x20 and forcealign = CCR.STKALIGN,
 * which we set to 1, so the pushed stack frame is aligned.
 **/

.section .text.vectors, "ax"
BEGIN_FUNC(arm_handle_exception)
    @ Get current task TCB
    ldr r0, =ksCurThread /* &NODE_STATE(ksCurThread) */
    ldr r0, [r0]
    @ offset to the kernel saved registers
    adds r0, #PT_R4
    @ save the PSP in r12 (arbitrarily, it is hardware-stacked)
    mrs r12, PSP
    @ store the kernel-saved (r4-r11), then the PSP into SP_process,
    @ then exc_return (in lr by hardware),
    stm r0, {r4-r11, r12, lr}

    bl c_handle_exception

    @ Get current task TCB (it might have changed)
    ldr r0, =ksCurThread /* &NODE_STATE(ksCurThread) */
    ldr r0, [r0]
    @ offset to the kernel saved registers
    adds r0, #PT_R4
    @ load the kernel-saved (r4-r11), the SP_process into r12,
    @ and "EXC_RETURN" into LR
    ldm r0, {r4-r11, r12, lr}
    @ write SP_process to PSP
    msr PSP, r12

    @ TODO: do we need isb to PSP write above?
    @ hardware will restore r0-r3, r12, and others
    bx lr
END_FUNC(arm_handle_exception)

BEGIN_FUNC(arm_SVCall_exception)
    @ Get current task TCB
    ldr r0, =ksCurThread /* &NODE_STATE(ksCurThread) */
    ldr r0, [r0]
    @ offset to the kernel saved registers
    adds r0, #PT_R4
    @ save the PSP in r12 (arbitrarily)
    mrs r12, PSP
    @ store the kernel-saved (r4-r11), then the PSP into SP_process,
    @ then exc_return (in lr by hardware),
    stm r0, {r4-r11, r12, lr}

    @ prepare arguments
    mov r0, r4  @ capRegister (TODO: assert)
    mov r1, r5  @ msgInfoRegister (TODO: assert)
    mov r2, r11 @ syscall_t
    bl c_handle_syscall

    @ Get current task TCB (it might have changed)
    ldr r0, =ksCurThread /* &NODE_STATE(ksCurThread) */
    ldr r0, [r0]
    @ offset to the kernel saved registers
    adds r0, #PT_R4
    @ load the kernel-saved (r4-r11), the SP_process into r12,
    @ and "EXC_RETURN" into LR
    ldm r0, {r4-r11, r12, lr}
    @ write SP_process to PSP
    msr PSP, r12

    @ hardware will restore r0-r3, r12, and others
    bx lr
END_FUNC(arm_SVCall_exception)

/**
 * We use the PendSV exception to handle context switching, as per the ARM
 * recommendations in "Use of SVCall and PendSV to avoid critical code regions"
 * on B1-530 of Armv7-M ARM DDI 0403E.e.
 * TODO do we though?
 * -> only for timers? because ticking. otherwise being able to only save
 * -> state sometimes doesn't really help much since it would be slower
 **/
BEGIN_FUNC(arm_PendSV_exception)
    b halt
END_FUNC(arm_PendSV_exception)
