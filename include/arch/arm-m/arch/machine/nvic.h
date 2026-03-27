#pragma once

#include <machine/interrupt.h>

// TODO!!!!!!!!!!!!!!!
static const irq_t irqInvalid = (uint16_t) -1;

static inline void maskInterrupt(bool_t disable, irq_t irq) {
    // TODO.
    assert(false);
}

static inline bool_t isIRQPending(void) {
    // TODO
    assert(false);
    return false;
}

static inline irq_t getActiveIRQ(void)
{
    assert(false);
    return 0;
}

static inline void ackInterrupt(irq_t irq)
{
    assert(false);
}

static inline void handleSpuriousIRQ(void)
{
    assert(false);
}
