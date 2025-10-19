#
# Copyright 2025, Julia V
#
# SPDX-License-Identifier: GPL-2.0-only
#

declare_platform(ast1030-evb KernelPlatformAst1030-evb PLAT_AST1030_EVB KernelArchARM-M)

if(KernelPlatformAst1030-evb)
    declare_seL4_arch(arm-m)

    set(KernelArmCortexM4F ON)
    set(KernelArchArmV7em ON)

    list(APPEND KernelDTSList "tools/dts/${KernelPlatform}.dts")
    list(APPEND KernelDTSList "src/plat/ast1030-evb/overlay-${KernelPlatform}.dts")

    declare_default_headers(
        TIMER_FREQUENCY 1
        # TODO:
        MAX_IRQ 100
        TIMER drivers/timer/arm_systick.h
        INTERRUPT_CONTROLLER arch/machine/nvic.h
    )
endif()

add_sources(
    DEP "KernelPlatformAst1030-evb"
    # CFILES src/arch/arm-m/machine/nvic.c src/arch/arm/machine/l2c_nop.c
)
