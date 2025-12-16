#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

declare_platform(mps2-an521 KernelPlatformMPS2-AN521 PLAT_MPS2_AN521 KernelArchArmM)

if(KernelPlatformMPS2-AN521)
    declare_seL4_arch(Armv8-M)

    set(KernelArmCortexM33 ON)
endif()

add_sources(
    DEP "KernelPlatformMPS2-AN521"
    CFILES
        src/drivers/serial/cmsdk-uart.c
        src/plat/mps2-an521/machine/io.c
    # CFILES src/arch/arm-m/machine/nvic.c src/arch/arm/machine/l2c_nop.c
)
