/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <machine/registerset.h>
#include <arch/machine/registerset.h>
#include <kernel/thread.h>

/**
 * For Arm-M architectures, the hardware does exception stacking/unstacking.
 * This means we need to move modified registers from the TCB into the hardware
 * stacked frame for exception return.
 **/
void Arch_postModifyRegisters(tcb_t *tptr)
{
    /* We need to copy the registers from our user_context into the PSP frame */

    // TODO: actually do.

    word_t SP = getRegister(tptr, SP_process);
    // TODO: check if PSP has gone away, lol.
    /// xxx: EXC_RETURN useful?
    assert(SP > 0x20000000 && SP < 0x20082000);

    /* The current stack pointer points to the bottom of the stack */
    word_t *frame = (word_t *)SP;
    /* Layout of the exception frame as per B3.19 of Armv8-M ARM DDI 0553B.y
     TODO: Armv7-M, other modes */
    frame[0] = getRegister(tptr, R0);
    frame[1] = getRegister(tptr, R1);
    frame[2] = getRegister(tptr, R2);
    frame[3] = getRegister(tptr, R3);
    frame[4] = getRegister(tptr, R12);
    frame[5] = getRegister(tptr, LR);
    frame[6] = getRegister(tptr, PC);
    frame[7] = getRegister(tptr, xPSR);
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

        // TODO: HACK
        exception_t status = (address > 0x20000000 && address < 0x20082000) ? EXCEPTION_NONE : EXCEPTION_FAULT;

        if (status == EXCEPTION_NONE) {
            word_t value = *(word_t *)(address);
            printf("0x%lx: 0x%lx\n", (long)address, (long)value);
        } else {
            printf("0x%lx: INVALID\n", (long)address);
        }
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
