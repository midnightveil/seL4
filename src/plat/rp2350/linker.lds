#include <config.h>
#define __ASSEMBLER__

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
    .boot.flash : {
        /* TODO: setup a picobin block to point to this elsewhere */
        /* This must be at the start of the image as used by rp2350 bootrom */
        KEEP(*(.vectors))

        /* This needs to be within the first 4k of the flash */
        KEEP(*(.picobin_blocks))

        *(.boot.rodata)
        *(.boot.text)
    } >FLASH

    .text : {
        *(.text)
        *(.text.*)
    } >FLASH

    .rodata : {
        *(.rodata) *(.rodata.*)

        /* Array of [word_t addr, word_t load_addr, word_t size][] */
        ki_relocation_regions_start = .;
        LONG(ADDR(.data)); LONG(LOADADDR(.data)); LONG(SIZEOF(.data));
        LONG(ADDR(.boot.sram)); LONG(LOADADDR(.boot.sram)); LONG(SIZEOF(.boot.sram));
        ki_relocation_regions_end = .;
    } >FLASH

    .bss (NOLOAD) : ALIGN(4) {
        ki_sram_start = .;
        ki_bss_start = .;

        /* Make running off the bottom of the stack fault with a BusError as it
           would be below SRAM */
        *(.kernel_stack);

        *(.bss)
        *(.bss.*)

        ki_bss_end = .;
    } >SRAM

    .data : ALIGN(4) {
        *(.data)
        *(.data.*)

        ki_sram_end = .;
    } >SRAM AT>FLASH

    .boot.sram : ALIGN(4) {
        ki_boot_sram_start = .;

        *(.boot.data)
        *(.boot.bss)

        ki_boot_sram_end = .;
    } >SRAM AT>FLASH

    /* Section for userspace application binaries to be loaded into flash.
     * The kernel will jump to 'ki_userspace_start'. The intention of this
     * section is to allow you to `objcopy` the userspace binaries into the
     * kernel ELF, leaving you with an ELF file which you can load to the
     * board.
     */
    .userspace : ALIGN(4) {
        ki_userspace_start = .;
    } >FLASH

    /DISCARD/ : {
        *(.ARM.exidx)
    }
}
