#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>
#include <arch/machine/hardware.h>

SECTIONS
{
    . = KERNEL_ELF_BASE;

    /* TODO: make this at an arbitrary address as
            On reset, the processor initializes the vector table base address to an IMPLEMENTATION DEFINED address
     */
    .isr_vector : {
        *(isr_vector)
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
        _start = .; /* make LLVM happy */
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
