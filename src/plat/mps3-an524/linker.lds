#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>

/*
 * Refer to «Application Note AN524 - Example SSE-200 Subsystem for MPS3»
 * Arm DAI 0524 Issue F
 * https://documentation-service.arm.com/static/60dca8af0320e92fa40b55dc
 *
 * Note that we use this in the "Remap" configuration, so as to better simulate
 * "real" hardware which stores the initial code as XIP in flash.
 */

/** This is important; the ARM FVP uses this address **/
/*ENTRY(arm_vector_table)*/

MEMORY
{
    /* Section 3.5 "Remap" memory layout */
    QSPI (rx) : ORIGIN = 0x0, LENGTH = 8M
    BRAM (rwx) : ORIGIN = 0x38000000, LENGTH = 512K
}

_qspi_start = ORIGIN(QSPI);
_qspi_end   = ORIGIN(QSPI) + LENGTH(QSPI);
_bram_start = ORIGIN(BRAM);
_bram_end   = ORIGIN(BRAM) + LENGTH(BRAM);

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
