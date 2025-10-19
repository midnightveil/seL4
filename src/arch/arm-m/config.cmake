#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

cmake_minimum_required(VERSION 3.16.0)

config_set(KernelHasVirtualMemory HAS_VIRTUAL_MEMORY OFF)

set(KernelFastpath OFF CACHE BOOL "")
set(KernelMaxNumBootinfoUntypedCaps 50 CACHE BOOL "")

add_sources(
    DEP "KernelArchARM-M"
    PREFIX src/arch/arm-m
    CFILES
        machine/hardware.c
        machine/nvic.c
        machine/registerset.c
        machine/thread.c
        api/faults.c
        halt.c
        # c_traps.c
        # benchmark/benchmark.c
        # kernel/boot.c
        kernel/thread.c
        # machine/cache.c
        # machine/errata.c
        # machine/debug.c
        # machine/hardware.c
        # machine/io.c
        object/objecttype.c
        object/interrupt.c
        object/tcb.c
    ASMFILES head.S
)

add_bf_source_old("KernelArchARM-M" "structures.bf" "include/arch/arm-m" "arch/object")
