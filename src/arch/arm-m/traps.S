/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <machine/assembler.h>
#include <arch/machine.h>
#include <util.h>

.syntax unified
.thumb

.extern restore_user_context
.extern kernel_stack_alloc

/**
 * Read the Armv7-M exception model, B1.5 of ARMv7-M ARM DDI 0403E.e.
 * The exception numbers and ordering of the vector table is defined in
 * Tables B1-4 and B1-5.
 **/
.section .isr_vector, "aw"
/**
 * > The Vector table must be naturally aligned to a power of two whose alignment
 * > value is greater than or equal to (Number of Exceptions supported x 4),
 * > with a minimum alignment of 128 bytes.
 * > All other entries must have bit[0] set to 1, because this bit
 * > defines the EPSR.T bit on exception entry.
 * TODO
 **/
.align 7 /* 2^7 */
.global arm_vector_table
arm_vector_table:
    /*  1 */ .word (kernel_stack_alloc + BIT(CONFIG_KERNEL_STACK_BITS))
    /*  2 */ .word arm_Reset_exception
    /*  3 */ .word arm_NMI_exception
    /*  4 */ .word arm_HardFault_exception
    /*  5 */ .word arm_MemManage_exception
    /*  6 */ .word arm_BusFault_exception
    /*  7 */ .word arm_UsageFault_exception
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

.section .temp, "ax"
BEGIN_FUNC(temp)
    1: b 1b
END_FUNC(temp)

.extern ksCurThread

.section .boot.text, "ax"
BEGIN_FUNC(arm_Reset_exception)
    bl init_kernel

    @ We can't use the standard restore_user_context() here because that
    @ assumes we are operating in Handler mode, where as when out of Reset
    @ we operate in Thread (privileged) mode.
    @
    @ We set up all the registers to empty values, then use control to
    @ switch to non-privileged mode.

    @ Load registers in the TCB which should be zeroed.
    ldr r0, =ksCurThread /* NODE_STATE(ksCurThread) */
    /* XXX: This is the frame size / offset of R4. TODO: asserts */
    @ note: r14 is LR
    adds r0, #0x20
    ldm r0, {r4-r11, r14}

    @ FPU restore, when desired.

    @ FIXME: Hack for now
    ldr r14, =temp

    msr PSP, r0

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
    @ the initial thread's LR register. but this is relatively unimportant.
    bx lr
END_FUNC(arm_Reset_exception)

/**
 * Each of the following exceptions behave following the ExceptionEntry()
 * psuedocode in §B1.5.6 ARMv7-M ARM DDI 0403E.e.
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
    b halt
END_FUNC(arm_SVCall_exception)

BEGIN_FUNC(arm_DebugMonitor_exception)
    b halt
END_FUNC(arm_DebugMonitor_exception)

/**
 * We use the PendSV exception to handle context switching, as per the ARM
 * recommendations in "Use of SVCall and PendSV to avoid critical code regions"
 * on B1-530 of ARMv7-M ARM DDI 0403E.e.
 **/
BEGIN_FUNC(arm_PendSV_exception)
    b halt
END_FUNC(arm_PendSV_exception)

BEGIN_FUNC(arm_SysTick_exception)
    b halt
END_FUNC(arm_SysTick_exception)

