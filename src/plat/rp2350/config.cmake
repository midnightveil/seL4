#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

declare_platform(rp2350 KernelPlatformRP2350 PLAT_RP2350 KernelArchArmM)

if(KernelPlatformRP2350)
    declare_seL4_arch(Armv8-M)

    set(KernelArmCortexM33 ON)
endif()

add_sources(
    DEP "KernelPlatformRP2350"
    ASMFILES
        src/plat/rp2350/head.S
    CFILES
        src/drivers/serial/pl011.c
        src/plat/rp2350/machine/io.c
    # CFILES src/arch/arm-m/machine/nvic.c src/arch/arm/machine/l2c_nop.c
)
