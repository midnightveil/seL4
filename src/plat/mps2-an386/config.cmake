#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

declare_platform(mps2-an386 KernelPlatformMPS2-AN386 PLAT_MPS2_AN386 KernelArchARM-M)

if(KernelPlatformMPS2-AN386)
    declare_seL4_arch(arm-m)

    set(KernelArmCortexM4F ON)
    set(KernelArchArmV7em ON)

    # declare_default_headers(
    #     TIMER_FREQUENCY 1
    #     # TODO:
    #     MAX_IRQ 100
    #     TIMER drivers/timer/arm_systick.h
    #     INTERRUPT_CONTROLLER arch/machine/nvic.h
    # )
endif()

add_sources(
    DEP "KernelPlatformMPS2-AN386"
    CFILES
        src/drivers/serial/cmsdk-uart.c
    # CFILES src/arch/arm-m/machine/nvic.c src/arch/arm/machine/l2c_nop.c
)
