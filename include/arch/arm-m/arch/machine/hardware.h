#pragma once

#include <plat/machine/devices_gen.h>

/* Kernel starts at the base of physical memory.
   XXXX: We assume that there is a mapping such that VTOR points at this
        at the start, or it's mapped so that 0x0 also points here (or is here).

    e.g. the ast1030-evb starts physmem at 0x0
    e.g. the MPS2 has ZBTSRAM 1 mapped at 0x0
*/
#define KERNEL_ELF_BASE PHYS_BASE_RAW
