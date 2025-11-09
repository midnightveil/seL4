#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>
#include <arch/machine/hardware.h>

/** This is important; the ARM FVP uses this address **/
ENTRY(arm_vector_table)

SECTIONS
{
    . = KERNEL_ELF_BASE;

    /* TODO: make this at an arbitrary address as
            On reset, the processor initializes the vector table base address to an IMPLEMENTATION DEFINED address
     */
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

    /DISCARD/ :
    {
        *(.note.gnu.build-id)
        *(.comment)
    }
}
