/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <kernel/thread.h>

void Arch_postModifyRegisters(tcb_t *tptr)
{
    /* We need to copy the registers from our user_context into the PSP frame */

    // TODO: check if PSP has gone away, lol.
    // TODO: actually do.

    fail("TODO");
}

void Arch_userStackTrace(tcb_t *tptr)
{
    printf("user stack trace\n");
    assert(!"TODO");
}
