/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <machine/assembler.h>
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
BEGIN_FUNC(arm_vector_table)
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
END_FUNC(arm_vector_table)

.section .boot.text, "ax"
BEGIN_FUNC(arm_Reset_exception)
    bl init_kernel

    /* Restore the initial thread. Note that the function restore_user_context()
     * could technically also be called at the end of init_kernel() directly,
     * there is no need to return to the assembly code here at all. However, for
     * verification things are a lot easier when init_kernel() is a normal C
     * function that returns. The function restore_user_context() is not a
     * normal C function and thus handled specially in verification, it does
     * highly architecture specific things to exit to user mode.
     */
    b restore_user_context
END_FUNC(arm_Reset_exception)

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

BEGIN_FUNC(arm_PendSV_exception)
    b halt
END_FUNC(arm_PendSV_exception)

BEGIN_FUNC(arm_SysTick_exception)
    b halt
END_FUNC(arm_SysTick_exception)

