/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <arch/machine/registerset.h>
#include <arch/types.h>
#include <arch/kernel/traps.h>
#include <config.h>
#include <model/statedata.h>
#include <util.h>

/** DONT_TRANSLATE */
void VISIBLE NORETURN restore_user_context(void)
{
    c_exit_hook();

    word_t user_context_regs = (word_t) &(NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers);

    // TODO: reprog MPU?

    asm volatile(
        /* set stack pointer to point at r0 of user context */
        "mov sp, %[user_context] \n"
        /* pop user registers off the stack */
        "pop {R0-R12}            \n"
    // TOOD: set nPRIV on CONTOLR?
        : /* no outputs */
        : [user_context] "r"(user_context_regs + NextIP * sizeof(word_t))
    );

    UNREACHABLE();
}
