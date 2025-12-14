#pragma once

#include <arch/machine.h>
#include <linker.h>

#define CPACR_CPn_MASK(n) (BIT(2 * n + 1) | BIT(2 * n))
#define CPACR_CPn_VALUE(n, v) (v << (2 * n))

/* "Fields CP10 and CP11 together control access to the Floating-point coprocessor, if implemented" */
#define CPACR_CP_FPU_MASK (CPACR_CPn_MASK(10) | CPACR_CPn_MASK(11))

/* Check for the presence of the optional floating point extensions FPv4-SP
   and FPv5. Reference A2.5 and B3.2.20 of Armv7-M ARM DDI 0403E.e.
*/
BOOT_CODE static inline bool_t fp_HWCapTest(void)
{

    /* Coprocessor Access Control Register CPACR, Armv7-M ARM DDI 0403E.e. §B3.2.20 */
    word_t CPACR = *SCS_CPACR;
    CPACR &= ~CPACR_CP_FPU_MASK;
    /* "To test whether a coprocessor is implemented, software can write 0b01 to
        a CPn field, and then read the CPACR. If the CPn field reads as zero the
        coprocessor is not implemented."
    */
    CPACR |= CPACR_CPn_VALUE(10, 0b01) | CPACR_CPn_VALUE(11, 0b01);
    *SCS_CPACR = CPACR;

    CPACR = *SCS_CPACR;

    return CPACR & CPACR_CP_FPU_MASK;
}

BOOT_CODE static inline void fp_HWDisable(void)
{
    /* Coprocessor Access Control Register CPACR, Armv7-M ARM DDI 0403E.e. §B3.2.20 */
    word_t CPACR = *SCS_CPACR;
    /* 0b00 = Access denied. Any attempted access generates a NOCP UsageFault. */
    CPACR &= ~CPACR_CP_FPU_MASK;
    *SCS_CPACR = CPACR;
}
