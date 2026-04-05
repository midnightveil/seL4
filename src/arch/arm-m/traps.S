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

    @ begin initial setup of the idle thread. most of the state has already
    @ been setup (e.g. MPU)

    @ TODO: I'm lazy.
    @ TODO: do we wanna switch around PSP/MSP? and xPSR etc?

    @ turn on interrupts. technically speaking we could be immediately interrupted
    @ if SysTick is pending between here and before we execute the branch
    @ instruction to the idle thread. but this is OK.
    cpsie i
    b idle_thread
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
