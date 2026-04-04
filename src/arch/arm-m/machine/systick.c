#include <stdint.h>
#include <linker.h>
#include <arch/machine/timer.h>

/**
 * Implementation of the Arm-M SysTick timer.
 *  B11 of Armv8-M ARM DDI 0553B.y ('The System Timer, SysTick')
 *  TODO ArmV7-M
 *
 * We assume we have at least the Main Extension, and per R_CNTG we have
 * at least one system timer implemented. Some baseline extensions without
 * the main extension may still have a system timer, however.
 **/

/**
 * D1.1.9 of Armv8-M ARM DDI 0553B.y
 * TODO ARMv7-M
 **/
#define SYST_BASE 0xE000E010
#define SYST_REG32(offset) (volatile uint32_t *)(SYST_BASE + offset)
#define SYST_CSR    SYST_REG32(0x0)
#define SYST_RVR    SYST_REG32(0x4)
#define SYST_CVR    SYST_REG32(0x8)
#define SYST_CALIB  SYST_REG32(0xC)

/* D1.2.240 of Armv8-M ARM DDI 0553B.y. Bits [23:0] */
#define SYST_CALIB_TENMS_MASK  MASK(24)

/* D1.2.241 of Armv8-M ARM DDI 0553B.y */
#define SYST_CSR_ENABLE    BIT(0)
#define SYST_CSR_TICKINT   BIT(1)
#define SYST_CSR_CLKSOURCE BIT(2)

BOOT_CODE void initTimer(void) {
    // TODO: use the kernel tick settings
    /* Set the reload value */
    *SYST_RVR = *SYST_CALIB & SYST_CALIB_TENMS_MASK;

    /* Enable the counter, enable the tick interrupt, use the PE clock */
    *SYST_CSR = SYST_CSR_ENABLE | SYST_CSR_TICKINT | SYST_CSR_CLKSOURCE;
    resetTimer();
}

static inline void resetTimer(void) {
    /* Per I_VHDT, writing to SYST_CVR clears SYST_CVR and SYST_CSR.COUNTFLAG.
       Any value can be written here and it will be cleared.
    */
    *SYST_CVR = 0x0;
}
