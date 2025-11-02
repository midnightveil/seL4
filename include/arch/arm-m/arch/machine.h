#pragma once

#include <arch/types.h>
#include <util.h>

/* Cleaning memory before user-level access. Does not flush cache. */
static inline void clearMemory(word_t *ptr, word_t bits)
{
    memzero(ptr, BIT(bits));
}

word_t PURE getRestartPC(tcb_t *thread);
void setNextPC(tcb_t *thread, word_t v);

/**
 * Move to Register from Special Register
 * B3.2.2 of ARMv7-M ARM DDI 0403E.e.
 */
#define MRS(spec_reg) ({ \
    word_t _v; \
    asm volatile("mrs %0, " spec_reg : "=r"(_v) ); \
    _v; \
})

/**
 * Move to Special Register from Arm Register
 * B3.2.3 of ARMv7-M ARM DDI 0403E.e.
 */
#define MSR(spec_reg, v) ({ \
    word_t _v = v; \
    asm volatile("msr " spec_reg ", %0" :: "r"(_v) ); \
    _v; \
})
