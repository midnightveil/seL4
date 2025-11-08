#pragma once

#ifndef __ASSEMBLER__

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

#endif

/**
 * ARMv7-M ARM DDI 0403E.e. §B3.2.2 System control and ID Registers, B3-596/7
 **/
#define SCS_BASE 0xE000ED00
#define SCS_REG32(offset) (volatile uint32_t *)(SCS_BASE + offset)

#define SCS_CPUID SCS_REG32(0x00)
#define SCS_ICSR  SCS_REG32(0x04)
#define SCS_VTOR  SCS_REG32(0x08)
#define SCS_AIRCR SCS_REG32(0x0C)
#define SCS_SCR   SCS_REG32(0x10)
#define SCS_CCR   SCS_REG32(0x14)
#define SCS_SHPR1 SCS_REG32(0x18)
#define SCS_SHPR2 SCS_REG32(0x1C)
#define SCS_SHPR3 SCS_REG32(0x20)
#define SCS_SHCSR SCS_REG32(0x24)
#define SCS_CFSR  SCS_REG32(0x28)
/// TODO: add __FSR registers
#define SCS_CPACR SCS_REG32(0x88)

#define CONTROL_nPRIV BIT(0)
#define CONTROL_SPSEL BIT(1)

#define xPSR_IPSR MASK(8)
#define xPSR_EPSR_T BIT(24)
