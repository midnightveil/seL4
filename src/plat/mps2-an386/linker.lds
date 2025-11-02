#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>
#include <arch/machine/hardware.h>


ENTRY(_start)

SECTIONS
{
    . = KERNEL_ELF_BASE;

    .isr_vector : {
        *(isr_vector)
    }

    .data : {
        *(.data)
    }

    .bss (NOLOAD): {
        *(.bss)
    }

    /* Main Stack Pointer (MSP), 4-byte aligned */
    .SP_main : ALIGN(4) {
        __stack_start = .;
        . = . + 2048;
        __stack_end = .;
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .text : {
        *(.text)
    }

    ki_end = .;

    ki_boot_start = .;

    .boot : {
        *(.boot.text)
        *(.boot.rodata)
        *(.boot.data)
    }

    ki_boot_end = .;

    /DISCARD/ :
    {
        *(.note.gnu.build-id)
        *(.comment)
    }
}
