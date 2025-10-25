#pragma once

#include <config.h>
#include <stdint.h>

typedef unsigned long word_t;
typedef signed long sword_t;
/* for printf() formatting */
#define _seL4_word_fmt  l

typedef word_t vptr_t;
typedef word_t paddr_t;
typedef word_t pptr_t;
typedef word_t cptr_t;
typedef word_t node_id_t;
typedef word_t cpu_id_t;
typedef word_t dom_t;

typedef struct kernel_frame {
    paddr_t paddr;
    word_t size;
    int armExecuteNever;
    int userAvailable;
} kernel_frame_t;
