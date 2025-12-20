#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>
#include <arch/machine/hardware.h>

/* Not the vector table because the RP2350 has a bootloader */
ENTRY(arm_Reset_exception)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 4M
    SRAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 520K
}

_flash_start = ORIGIN(FLASH);
_flash_end   = ORIGIN(FLASH) + LENGTH(FLASH);
_sram_start  = ORIGIN(SRAM);
_sram_end    = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
    __sram_start : {
        ki_start = .;
    } > SRAM

    .text :
    {
        /* This must be at the start of the image as used by rp2350 bootrom */
        KEEP(*(.vectors))
        /* This needs to be within the first 4k of the flash */
        KEEP(*(.picobin_blocks))

        *(.text)
        *(.text.*)

        /* There's no point boot.text being separate because it has to live
           in flash anyway. */
        *(.boot.text)
    } >SRAM

    .data : {
        *(.data)
        *(.data.*)
    } >SRAM

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } >SRAM

    /* word-sized alignment for memzero */
    .bss : ALIGN(32) {
        ki_bss_start = .;

        *(.bss)
        *(.bss.*)
        *(COMMON)

        ki_bss_end = .;
    } >SRAM

    .boot : {
        ki_boot_start = .;

        *(.boot.data)
        *(.boot.rodata)
        *(.boot.bss)

        ki_boot_end = .;
    } >SRAM

    /* TEMP HACK */
    . = . + 0x1000;
    .user : {
        *(.user)
        *(.user.*)
    } >SRAM

    __sram_end : {
        ki_end = .;
    } >SRAM

    /DISCARD/ : {
        *(.ARM.exidx)
    }
}
