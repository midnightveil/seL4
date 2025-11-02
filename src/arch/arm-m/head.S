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

.section .boot.text, "ax"
.thumb_func
BEGIN_FUNC(_start)
    b init_kernel
END_FUNC(_start)

.section .isr_vector, "aw"
.align 2
BEGIN_FUNC(arm_vector_table)
    .word __stack_start
    .word _start
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
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    # .word systick_handler
    # .word irq_handler_1
END_FUNC(arm_vector_table)
