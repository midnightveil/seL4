/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <machine/assembler.h>

/* https://community.arm.com/arm-community-blogs/b/architectures-and-processors-blog/posts/writing-your-own-startup-code-for-cortex-m */

.syntax unified
.thumb

// TODO: https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-processor/exception-model/vector-table

.section .isr_vector, "aw"
.align 2
BEGIN_FUNC(arm_vector_table)
    .word __stack_start
    .word _start
    .word arm_nmi_handler
    .word arm_hard_fault_handler
    .word arm_memory_management_fault
    .word arm_bus_fault
    .word arm_usage_fault
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    # .word systick_handler
    # .word irq_handler_1
END_FUNC(arm_vector_table)

.section .boot.text, "ax"
BEGIN_FUNC(_start)
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
END_FUNC(_start)
