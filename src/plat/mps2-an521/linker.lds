#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>

/*
 * Refer to «Application Note AN521 - Example SSE-200 Subsystem for MPS2+»
 * Arm DAI 0521 Issue C
 * https://documentation-service.arm.com/static/5fa12fe9b1a7c5445f29017f
 */

/** This is important; the ARM FVP uses this address **/
ENTRY(arm_vector_table)

MEMORY
{
    /* 3.4.1 External ZBT SRAMs Synchronous SRAM for Code (SSRAM1) - Secure alias */
    SSRAM1 (rx) : ORIGIN = 0x10000000, LENGTH = 4M
    /* 3.4.2 External ZBT SRAMs Synchronous SRAM (SSRAM2 and SSRAM3) */
    SSRAM2_3 (rw) : ORIGIN = 0x38000000, LENGTH = 4M
    /* 3.4.3 PSRAM */
    PSRAM (rw)  : ORIGIN = 0x80000000, LENGTH = 16M

    NS_SSRAM1 (rx) : ORIGIN = 0x0, LENGTH = 4M
}

_ssram1_start = ORIGIN(SSRAM1);
_ssram1_end   = ORIGIN(SSRAM1) + LENGTH(SSRAM1);
_ssram2_3_start = ORIGIN(SSRAM2_3);
_ssram2_3_end   = ORIGIN(SSRAM2_3) + LENGTH(SSRAM2_3);
_psram_start  = ORIGIN(PSRAM);
_psram_end    = ORIGIN(PSRAM) + LENGTH(PSRAM);

SECTIONS
{
    ki_start = .;

    .vectors : {
        *(.vectors)
    } > SSRAM1

    .data : {
        *(.data)
    }

    .bss (NOLOAD): {
        *(.bss)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .text : {
        *(.text)
        *(.text.*)
    }

    ki_end = .;


    .boot : {
        ki_boot_start = .;

        *(.boot)
        *(.boot.*)

        ki_boot_end = .;
    }


    /* TEMP HACK */
    . = . + 0x1000;
    .user : {
        *(.user)
        *(.user.*)
    } > NS_SSRAM1
}
