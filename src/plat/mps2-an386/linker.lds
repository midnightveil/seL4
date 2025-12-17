#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>

/**
# Fast Models Fixed Virtual Platforms in Arm Development
# Studio Reference Guide
# Document ID: 110379_1129_00_en
# Issue 00
# MPS2
# 5. MPS2
# The Microcontroller Prototyping System 2 (MPS2) Fixed Virtual Platforms (FVPs) implement
# a subset of the functionality of the Arm Versatile Express V2M-MPS2 and V2M-MPS2+
# motherboard hardware.
# DAI0386C_cortex_m4_on_v2m_mps2 for the cortex m4
*/

/** This is important; the ARM FVP uses this address **/
ENTRY(arm_vector_table)

MEMORY
{
    ZBTSRAM1 (rwx)  : ORIGIN = 0x0, LENGTH = 4M
    ZBTSRAM2 (rwx)  : ORIGIN = 0x20000000, LENGTH = 4M
}

_zbtsram1_start = ORIGIN(ZBTSRAM1);
_zbtsram1_end   = ORIGIN(ZBTSRAM1) + LENGTH(ZBTSRAM1);
_zbtsram2_start = ORIGIN(ZBTSRAM2);
_zbtsram2_end   = ORIGIN(ZBTSRAM2) + LENGTH(ZBTSRAM2);

SECTIONS
{
    ki_start = .;

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
