#include <config.h>
#define __ASSEMBLER__

#include <hardware.h>
#include <arch/machine/hardware.h>

ENTRY(_start)

SECTIONS
{
    . = KERNEL_ELF_BASE;

    .boot . : {
        *(.boot.text)
        *(.boot.rodata)
        *(.boot.data)
        . = ALIGN(64K);
    }

    ki_boot_end = .;

    .text . : {
        /* Sit inside a large frame */
        . = ALIGN(64K);
        *(.vectors)

        /* Fastpath code */
        *(.vectors.fastpath_call)
        *(.vectors.fastpath_reply_recv)
        *(.vectors.text)

        /* Anything else that should be in the vectors page. */
        *(.vectors.*)

        /* Hopefully all that fits into 4K! */

        /* Standard kernel */
        *(.text)
    }

    .rodata . : {
        *(.rodata)
        *(.rodata.*)
    }

    .data . : {
        *(.data)
    }

    . = ALIGN(4K);

    .bss . (NOLOAD): {
        *(.bss)
        *(COMMON) /* fallback in case '-fno-common' is not used */

        /* 4k abort stack, needed for PMODE_ABORT on ARM32 */
        _abort_stack_bottom = .;
        . = . + 4K ;
        _abort_stack_top = .;

        /* large data such as the globals frame and global PD */
        *(.bss.aligned)
    }

    . = ALIGN(4K);
    ki_end = .;

    /DISCARD/ :
    {
        *(.note.gnu.build-id)
        *(.comment)
    }
}
