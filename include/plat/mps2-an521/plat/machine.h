/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#ifndef __ASSEMBLER__

#include <config.h>
#include <linker.h>         /* for BOOT_RODATA */
#include <basic_types.h>    /* for p_region_t, kernel_frame_t (arch/types.h) */

#include <arch/machine/nvic.h>

// #define TIMER_CLOCK_HZ ULL_CONST(@CONFIGURE_TIMER_FREQUENCY@)

// TODO: These numbers are fake

enum IRQConstants {
    maxIRQ                      = 100
};

#define IRQ_CNODE_SLOT_BITS (7)


#ifdef CONFIG_PRINTING
#define UART_PADDR 0x40200000
#define UART_PPTR UART_PADDR
#endif

/* ARM-M specific (TODO) */
#define NVIC_PADDR 0xe000e100
#define SYSTICK_PADDR 0xe000e010

static const kernel_frame_t BOOT_RODATA kernel_device_frames[] = {
    #ifdef CONFIG_PRINTING
    {
        .paddr = UART_PADDR,
        .size = 0x1000,
        .userAvailable = true
    },
    #endif
    {
        .paddr = NVIC_PADDR,
        .size = 0xc00,
        .userAvailable = false
    },
    {
        .paddr = SYSTICK_PADDR,
        .size = 0x10,
        .userAvailable = false
    },
};

/* Elements in kernel_device_frames may be enabled in specific configurations
 * only, but the ARRAY_SIZE() macro will automatically take care of this.
 * However, one corner case remains unsolved where all elements are disabled
 * and this becomes an empty array effectively. Then the C parser used in the
 * formal verification process will fail, because it follows the strict C rules
 * which do not allow empty arrays. Luckily, we have not met this case yet...
 */
#define NUM_KERNEL_DEVICE_FRAMES ARRAY_SIZE(kernel_device_frames)

extern char _ssram1_start[1], _ssram1_end[1];
extern char _ssram2_3_start[1], _ssram2_3_end[1];
extern char _psram_start[1], _psram_end[1];

/* PHYSICAL MEMORY */
static const p_region_t BOOT_RODATA avail_p_regs[] = {
    { .start = (paddr_t)&_ssram1_start, .end = (paddr_t)&_ssram1_end },
    { .start = (paddr_t)&_ssram2_3_start, .end = (paddr_t)&_ssram2_3_end },
    { .start = (paddr_t)&_psram_start, .end = (paddr_t)&_psram_end }
};

#endif /* !__ASSEMBLER__ */
