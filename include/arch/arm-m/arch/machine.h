#pragma once

#ifndef __ASSEMBLER__

#include <arch/types.h>
#include <basic_types.h>
#include <linker.h>
#include <object/structures.h>
#include <util.h>

/* Cleaning memory before user-level access. Does not flush cache. */
static inline void clearMemory(word_t *ptr, word_t bits)
{
    memzero(ptr, BIT(bits));
}

word_t PURE getRestartPC(tcb_t *thread);
void setNextPC(tcb_t *thread, word_t v);
void setNextSP(tcb_t *thread, word_t v);

static inline void dsb(void)
{
    asm volatile("dsb" ::: "memory");
}

static inline void isb(void)
{
    asm volatile("isb" ::: "memory");
}

/**
 * Move to Register from Special Register
 * B3.2.2 of Armv7-M ARM DDI 0403E.e
 * C2.4.125 of Armv8-M ARM DDI 0553B.y
 */
#define MRS(spec_reg) ({ \
    word_t _v; \
    asm volatile("mrs %0, " spec_reg : "=r"(_v) ); \
    _v; \
})

/**
 * Move to Special Register from Arm Register
 * B3.2.3 of Armv7-M ARM DDI 0403E.e.
 * C2.4.126 of Armv8-M ARM DDI 0553B.y
 *
 * Usually, this requires a Context synchronisation event to guarantee that
 * changes to the special register affect instructions executing later in
 * program order.
 */
#define MSR(spec_reg, v) ({ \
    word_t _v = v; \
    asm volatile("msr " spec_reg ", %0" :: "r"(_v) ); \
    _v; \
})

/**
 * Armv7-M ARM DDI 0403E.e B3.2 System Control Space (SCS)
 *  > address range 0xE000E000 to 0xE000EFFF
 *
 * Armv8-M ARM DDI 0553B.y B8.3 The System Control Space (SCS);
 * specifically B8.2 The System region of the system address map (R_MHGM)
 *  > address range 0xE000E000 to 0xE000EFFF
 *
 * Unprivileged accesses to these registers are either RAZ/WI or generate a
 * fault, so we don't need to explicitly setup memory protection for these
 * registers.
 **/
static const p_region_t system_control_space_reg BOOT_RODATA = {
    .start = 0xE000E000,
    .end   = 0xE000EFFF,
};

/**
 *
 * System Control Block, part of the System Control Space
 *
 * B3.2.2 'Table B3-4 Summary of SCB registers' of Armv8-M DDI 0403E.e
 * D1.1.11 'Register Index: System Control Block' of Armv8-M ARM DDI 0553B.y
 *
 * Per (Armv8-M) B8.3 The System Control Space, the side-effects of accesses
 * to SCS (and by extension, SCB) registers can be guaranteed with a DSB
 * for completion, and then a Context synchronisation event for visibility
 * for instructions later in program order.
 *
 **/
#define SCB_BASE 0xE000ED00
#define SCB_REG32(offset) (volatile uint32_t *)(SCB_BASE + offset)

#define SCB_CPUID SCB_REG32(0x00)
#define SCB_ICSR  SCB_REG32(0x04)
#define SCB_VTOR  SCB_REG32(0x08)
#define SCB_AIRCR SCB_REG32(0x0C)
#define SCB_SCR   SCB_REG32(0x10)
#define SCB_CCR   SCB_REG32(0x14)
#define SCB_SHPR1 SCB_REG32(0x18)
#define SCB_SHPR2 SCB_REG32(0x1C)
#define SCB_SHPR3 SCB_REG32(0x20)
#define SCB_SHCSR SCB_REG32(0x24)
#define SCB_CFSR  SCB_REG32(0x28)
#define SCB_HFSR  SCB_REG32(0x2C)
#define SCB_DFSR  SCB_REG32(0x30)
#define SCB_MMFAR SCB_REG32(0x34)
#define SCB_BFAR  SCB_REG32(0x38)
#define SCB_AFSR  SCB_REG32(0x3C)
#define SCB_CPACR SCB_REG32(0x88)

#endif

#define CONTROL_nPRIV BIT(0)
#define CONTROL_SPSEL BIT(1)

#define xPSR_IPSR MASK(8)
#define xPSR_EPSR_T BIT(24)

// TODO: ARMv7

/* D1.2.11 of Armv8-M ARM DDI 0553B.y */
#define CFSR_BFSR_EXTRACT(CFSR)  ((CFSR >> 8) & 0xff)
#define CFSR_MMFSR_EXTRACT(CFSR) ((CFSR >> 0) & 0xff)
#define CFSR_UFSR_EXTRACT(CFSR)  ((CFSR >> 16) & 0xffff)

/* D1.2.167 of Armv8-M ARM DDI 0553B.y */
#define MMFSR_MMFARVALID BIT(7)
/* D1.2.7 of Armv8-M ARM DDI 0553B.y */
#define BFSR_BFARVALID BIT(7)

// TODO: ref
#ifdef CONFIG_ARCH_ARMV8M
#define SHCSR_SECUREFAULTENA BIT(19)
#endif
#define SHCSR_USGFAULTENA BIT(18)
#define SHCSR_BUSFAULTENA BIT(17)
#define SHCSR_MEMFAULTENA BIT(16)
