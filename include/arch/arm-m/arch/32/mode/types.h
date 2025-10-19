#pragma once

#include <assert.h>
#include <stdint.h>

compile_assert(long_is_32bits, sizeof(unsigned long) == 4)

#define wordRadix 5
#define wordBits (1 << wordRadix)

typedef uint32_t timestamp_t;
