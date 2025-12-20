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

#ifdef CONFIG_PRINTING
void Arch_userStackTrace(tcb_t *tptr)
{
    word_t sp = getRegister(tptr, SP_process);
    if (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP not aligned\n");
        return;
    }

    for (word_t i = 0; i < CONFIG_USER_STACK_TRACE_LENGTH; i++) {
        word_t address = sp + (i * sizeof(word_t));
        /* TODO: validate address */
        word_t value = *(word_t *)(address);
            printf("0x%lx: 0x%lx\n", (long)address, (long)value);
#if 0
        if (result.status == EXCEPTION_NONE) {
        } else {
            printf("0x%lx: INVALID\n", (long)address);
        }
#endif
    }
}
#endif

void Arch_prepareNextDomain(void)
{
    /* Don't need to do anything */
}

void Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
