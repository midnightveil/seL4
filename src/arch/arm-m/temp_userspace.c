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
    for (int v = 0; v < 1000; v++) {
        asm volatile(
            "mov r4, %[v]  \n"
            "mov r5, %[v]  \n"
            /* make a SysCall */
            "mov r11, #-1    \n"
            /* this syscall */
            "svc #0          \n"
            :
            : [v] "r"(v)
            : "memory", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9",
              "r10", "r11", "r12"
        );
    }

    for (;;) {}
}
