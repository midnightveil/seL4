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
    /* 3.4.1 External ZBT SRAMs Synchronous SRAM for Code (SSRAM1) */
    SSRAM1 (rx) : ORIGIN = 0x0, LENGTH = 4M
    /* 3.4.2 External ZBT SRAMs Synchronous SRAM (SSRAM2 and SSRAM3) */
    SSRAM2 (rw) : ORIGIN = 0x28000000, LENGTH = 4M
    SSRAM3 (rw) : ORIGIN = 0x28200000, LENGTH = 4M
    /* 3.4.3 PSRAM */
    PSRAM (rw)  : ORIGIN = 0x80000000, LENGTH = 16M
}

_ssram1_start = ORIGIN(SSRAM1);
_ssram1_end   = ORIGIN(SSRAM1) + LENGTH(SSRAM1);
_ssram2_start = ORIGIN(SSRAM2);
_ssram2_end   = ORIGIN(SSRAM2) + LENGTH(SSRAM2);
_ssram3_start = ORIGIN(SSRAM3);
_ssram3_end   = ORIGIN(SSRAM3) + LENGTH(SSRAM3);
_psram_start  = ORIGIN(PSRAM);
_psram_end    = ORIGIN(PSRAM) + LENGTH(PSRAM);

SECTIONS
{
    .vectors : {
        *(.vectors)
    }

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
    }
}
