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

/**
 * Read the Armv7-M exception model, B1.5 of Armv7-M ARM DDI 0403E.e.
 * The exception numbers and ordering of the vector table is defined in
 * Tables B1-4 and B1-5.
 *
 * > The Vector table must be naturally aligned to a power of two whose alignment
 * > value is greater than or equal to (Number of Exceptions supported x 4),
 * > with a minimum alignment of 128 bytes.
 * > All other entries must have bit[0] set to 1, because this bit
 * > defines the EPSR.T bit on exception entry.
 * TODO
 **/
.section .vectors, "aw"
.align 7 /* 2^7 */
.global arm_vector_table
arm_vector_table:
    /*  0 */ .word (kernel_stack_alloc + BIT(CONFIG_KERNEL_STACK_BITS))
    /*  1 */ .word arm_Reset_exception
    /*  2 */ .word arm_NMI_exception
    /*  3 */ .word arm_HardFault_exception
    /*  4 */ .word arm_MemManage_exception
    /*  5 */ .word arm_BusFault_exception
    /*  6 */ .word arm_UsageFault_exception
    /*  7 */ .word 0 /* Reserved */
    /*  8 */ .word 0 /* Reserved */
    /*  9 */ .word 0 /* Reserved */
    /* 10 */ .word 0 /* Reserved */
    /* 11 */ .word arm_SVCall_exception
    /* 12 */ .word arm_DebugMonitor_exception
    /* 13 */ .word 0 /* Reserved */
    /* 14 */ .word arm_PendSV_exception
    /* 15 */ .word arm_SysTick_exception

    .word 0 /* External interrupt 0 */
    /* etc etc */
    /* TODO: pad to # interrupts */
.size arm_vector_table, .-arm_vector_table

.extern ksCurThread

.section .boot.text, "ax"
BEGIN_FUNC(arm_Reset_exception)
    /* Not all platforms will look at the vector table to set the stack */
    ldr sp, =(kernel_stack_alloc + BIT(CONFIG_KERNEL_STACK_BITS))

    /* zero out BSS, we can't do this in C as we can't use the stack */
    ldr r0, =ki_bss_start
    ldr r1, =ki_bss_end
    /* fill value */
    ldr r2, #0
_zero_bss_start:
    cmp r1, r0
    /* if r1 > r0, break */
    bhi _zero_bss_done
    /* zero out r0 */
    str r2, [r0]
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
    @ offset to the kernel saved
    adds r0, #PT_R4
    @ load the kernel-saved (r4-r11), the SP_process into r12, and "EXC_RETURN"
    @ which we abuse for the initial task to mean the start program counter
    ldm r0, {r4-r11, r12, lr}
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

    @ unfortunately this is a branch-exchange which leaks the current PC int
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
BEGIN_FUNC(arm_NMI_exception)
    b halt
END_FUNC(arm_NMI_exception)

BEGIN_FUNC(arm_HardFault_exception)
    b halt
END_FUNC(arm_HardFault_exception)

BEGIN_FUNC(arm_MemManage_exception)
    b halt
END_FUNC(arm_MemManage_exception)

BEGIN_FUNC(arm_BusFault_exception)
    b halt
END_FUNC(arm_BusFault_exception)

BEGIN_FUNC(arm_UsageFault_exception)
    b halt
END_FUNC(arm_UsageFault_exception)

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
    @ write SP_proces to PSP
    msr PSP, r12

    @ hardware will restore r0-r3, r12, and others
    bx lr
END_FUNC(arm_SVCall_exception)

BEGIN_FUNC(arm_DebugMonitor_exception)
    b halt
END_FUNC(arm_DebugMonitor_exception)

/**
 * We use the PendSV exception to handle context switching, as per the ARM
 * recommendations in "Use of SVCall and PendSV to avoid critical code regions"
 * on B1-530 of Armv7-M ARM DDI 0403E.e.
 **/
BEGIN_FUNC(arm_PendSV_exception)
    b halt
END_FUNC(arm_PendSV_exception)

BEGIN_FUNC(arm_SysTick_exception)
    b halt
END_FUNC(arm_SysTick_exception)

