#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

declare_platform(mps2-an386 KernelPlatformMPS2-AN386 PLAT_MPS2_AN386 KernelArchArmM)

if(KernelPlatformMPS2-AN386)
    declare_seL4_arch(Armv7-M)

    set(KernelArmCortexM4F ON)
endif()

add_sources(
    DEP "KernelPlatformMPS2-AN386"
    CFILES
        src/drivers/serial/cmsdk-uart.c
        src/plat/mps2-an386/machine/io.c
    # CFILES src/arch/arm-m/machine/nvic.c src/arch/arm/machine/l2c_nop.c
)
