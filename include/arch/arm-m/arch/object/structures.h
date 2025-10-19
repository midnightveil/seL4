#pragma once

#define tcbArchCNodeEntries tcbCNodeEntries

static inline bool_t CONST Arch_isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    switch (cap_get_capType(derivedCap)) {
    default:
        return false;
    }
}
