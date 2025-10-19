#pragma once

#include <basic_types.h>
#include <util.h>
#include <arch/machine/registerset.h>
#include <arch/object/structures_gen.h>

#define tcbArchCNodeEntries tcbCNodeEntries

typedef struct arch_tcb {
    /* saved user-level context of thread */
    user_context_t tcbContext;
} arch_tcb_t;

static inline bool_t CONST Arch_isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    switch (cap_get_capType(derivedCap)) {
    default:
        return false;
    }
}

static inline void *CONST cap_get_archCapPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {
        /* TODO */

    default:
        /* Unreachable, but GCC can't figure that out */
        return NULL;
    }
}

static inline bool_t CONST cap_get_archCapIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {


    default:
        /* Unreachable, but GCC can't figure that out */
        return false;
    }
}


static inline word_t CONST cap_get_archCapSizeBits(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {
    default:
        /* Unreachable, but GCC can't figure that out */
        return 0;
    }
}
