#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

cmake_minimum_required(VERSION 3.16.0)

config_set(KernelHasVirtualMemory HAS_VIRTUAL_MEMORY OFF)

# TODO: should be this but this appears to... because we use the [start, end)
#       rather than [start, end] in the boot code this breaks
# math(EXPR KernelPaddrUserTop "(1 << 32) - 1")
math(EXPR KernelPaddrUserTop "(1 << 31)")

# Size!
set(KernelOptimisation "-Os" CACHE STRING "")

set(KernelFastpath OFF CACHE BOOL "")
set(KernelMaxNumBootinfoUntypedCaps 50 CACHE STRING "")

add_sources(
    DEP "KernelArchArmv7M"
    PREFIX src/arch/Armv7-M
    CFILES
        machine/hardware.c
        machine/nvic.c
        machine/registerset.c
        machine/thread.c
        api/faults.c
        halt.c
        c_traps.c
        # benchmark/benchmark.c
        kernel/boot.c
        kernel/thread.c
        # machine/cache.c
        # machine/errata.c
        # machine/debug.c
        # machine/hardware.c
        machine/io.c
        object/objecttype.c
        object/interrupt.c
        object/tcb.c

        temp_userspace.c

    ASMFILES idle.S traps.S
)

add_bf_source_old("KernelArchArmv7M" "structures.bf" "include/arch/Armv7-M" "arch/object")
