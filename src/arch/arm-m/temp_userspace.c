/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <util.h>
#include <arch/types.h>

void _user_main(void) SECTION(".user.main");

char _user_stack[0x400] SECTION(".user.stack");
word_t _user_stack_top = (word_t)&_user_stack[0x400 - 2 * sizeof(word_t)];

void _user_main(void) {
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += 1;
    }

    asm volatile(
        "mov r0, %[sum]  \n"
        /* this syscall */
        "svc #0          \n"
        :
        : [sum] "r"(sum)
        : "memory"
    );
}
