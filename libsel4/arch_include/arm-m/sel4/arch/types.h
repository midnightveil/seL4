/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sel4/macros.h>
#include <sel4/simple_types.h>
#include <sel4/sel4_arch/types.h>

// XXX
// typedef seL4_CPtr seL4_ARM_Page;

typedef struct seL4_UserContext_ {
    seL4_Word r0;
    seL4_Word r1;
    seL4_Word r2;
    seL4_Word r3;
    seL4_Word r4;
    seL4_Word r5;
    seL4_Word r6;
    seL4_Word r7;
    seL4_Word r8;
    seL4_Word r9;
    seL4_Word r10;
    seL4_Word r11;
    seL4_Word r12;

    seL4_Word psp; /* r13 (process stack pointer) */
    seL4_Word lr;  /* r14 (link register) */
    seL4_Word pc;  /* r15 (program counter) */

    seL4_Word xPSR;
} seL4_UserContext;
