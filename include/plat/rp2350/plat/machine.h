#pragma once

/* ARM-M specific (TODO) */
#define NVIC_PADDR 0xe000e100
#define SYSTICK_PADDR 0xe000e010

#ifdef CONFIG_PRINTING
/* UART0_BASE */
#define UART_PADDR 0x40070000
#define UART_PPTR UART_PADDR
#endif

#ifndef __ASSEMBLER__

#include <config.h>

#include <arch/machine/nvic.h>

// #define TIMER_CLOCK_HZ ULL_CONST(@CONFIGURE_TIMER_FREQUENCY@)

// TODO: These numbers are fake

enum IRQConstants {
    maxIRQ                      = 100
};

#define IRQ_CNODE_SLOT_BITS (7)

#endif /* !__ASSEMBLER__ */
