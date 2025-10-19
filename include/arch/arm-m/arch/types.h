#pragma once

#include <config.h>
#include <stdint.h>

typedef uint32_t word_t;
typedef int32_t sword_t;
/* for printf() formatting */
#define _seL4_word_fmt  l

typedef word_t vptr_t;
typedef word_t paddr_t;
typedef word_t pptr_t;
typedef word_t cptr_t;
typedef word_t node_id_t;
typedef word_t cpu_id_t;
typedef word_t dom_t;
