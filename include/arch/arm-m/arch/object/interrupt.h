#pragma once

#include <types.h>
#include <api/failures.h>
#include <object/structures.h>
#include <machine/interrupt.h>
#include <plat/machine.h>

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer);


/* Handle a platform-reserved IRQ. */
static inline void handleReservedIRQ(irq_t irq)
{
#ifdef CONFIG_IRQ_REPORTING
    printf("Received unhandled reserved IRQ: 0x%lx\n", IRQT_TO_IRQ(irq));
#endif
}


static inline exception_t Arch_checkIRQ(word_t irq_w)
{
    if (irq_w > maxIRQ) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [0..%u].", (int)irq_w, maxIRQ);
        return EXCEPTION_SYSCALL_ERROR;
    }
    return EXCEPTION_NONE;
}

