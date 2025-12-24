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
.extern ksCurThread

.section .boot.text, "ax"
BEGIN_FUNC(arm_Reset_exception)
    /* Not all platforms will look at the vector table to set the stack */
    ldr sp, =(kernel_stack_alloc + BIT(CONFIG_KERNEL_STACK_BITS))

    /* zero out BSS, we can't do this in C as we can't use the stack */
    ldr r0, =ki_bss_start
    ldr r1, =ki_bss_end
    /* fill value */
    movs r2, #0
_zero_bss_start:
    cmp r0, r1
    /* exit out if r0 >= r1 */
    bhs _zero_bss_done
    /* zero out r0; word-sized stores */
    str r2, [r0]
    /* r0 += 4 */
    adds r0, r0, #4
    b _zero_bss_start
_zero_bss_done:

    bl init_kernel

    @ We can't use the standard restore_user_context() here because that
    @ assumes we are operating in Handler mode, where as when out of Reset
    @ we operate in Thread (privileged) mode.
    @
    @ We set up all the registers to empty values, then use control to
    @ switch to non-privileged mode.

    @ Load registers from the TCB
    ldr r0, =ksCurThread /* &NODE_STATE(ksCurThread) */
    ldr r0, [r0]
    @ grab the NextIP for the notionally-hardware saved registers, which we've
    @ set with the value for the initial task
    ldr lr, [r0, #PT_NextIP]
    @ offset to the kernel saved
    adds r0, #PT_R4
    @ load the kernel-saved (r4-r11), the SP_process into r12, and ignore
    @ EXC_RETURN.
    ldm r0, {r4-r11, r12}
    @ write SP_process to PSP
    msr PSP, r12

    @ FPU restore, when desired.

    @ TODO: turn on MPU.

    @ Set up: thread mode unprivileged using SP_process
    movs r0, #:lower0_7:(CONTROL_nPRIV | CONTROL_SPSEL)
    msr CONTROL, r0
    @ "Software must use an ISB barrier instruction to ensure a
    @  write to the CONTROL register" (per §B1.4.4)
    isb

    @ zero out r0, r1, r2, r3, r12
    movs r0, #0
    movs r1, #0
    movs r2, #0
    movs r3, #0
    movs r12, #0

    @ unfortunately this is a branch-exchange which leaks the current PC into
    @ the initial thread's LR (R14) register. but this is relatively unimportant.
    bx lr
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

    b c_handle_exception

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
    b c_handle_syscall

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

BEGIN_FUNC(arm_SysTick_exception)
    b halt
END_FUNC(arm_SysTick_exception)

