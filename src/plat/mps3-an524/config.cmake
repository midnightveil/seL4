#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

declare_platform(mps3-an524 KernelPlatformMPS3-AN524 PLAT_MPS3_AN524 KernelArchArmM)

if(KernelPlatformMPS3-AN524)
    declare_seL4_arch(Armv8-M)

    set(KernelArmCortexM33 ON)
endif()

add_sources(
    DEP "KernelPlatformMPS3-AN524"
    CFILES
        src/drivers/serial/cmsdk-uart.c
        src/plat/mps3-an524/machine/io.c
    # CFILES src/arch/arm-m/machine/nvic.c src/arch/arm/machine/l2c_nop.c
)
