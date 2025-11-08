/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <util.h>

void _user_main(void) SECTION(".user.main");

char _user_stack[0x400] SECTION(".user.stack");

void _user_main(void) {
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += 1;
    }

    asm volatile(
        "mov r0, %[sum]  \n"
        /* this should panic */
        "svc #0xff       \n"
        :
        : [sum] "r"(sum)
        : "memory"
    );
}
