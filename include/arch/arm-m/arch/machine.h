#pragma once

#include <arch/types.h>
#include <util.h>

/* Cleaning memory before user-level access. Does not flush cache. */
static inline void clearMemory(word_t *ptr, word_t bits)
{
    memzero(ptr, BIT(bits));
}
