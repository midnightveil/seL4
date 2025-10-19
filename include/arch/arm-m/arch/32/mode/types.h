#pragma once

#include <config.h>
#include <assert.h>
#include <stdint.h>

compile_assert(uint32_is_32bits, sizeof(uint32_t) == 4)

#define wordRadix 5
