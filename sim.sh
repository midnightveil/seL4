#!/usr/bin/env sh

set -euo pipefail

# qemu-system-arm -M ast1030-evb -nographic -kernel "$@"
qemu-system-arm -M mps2-an386 -nographic -kernel "$@"

# use --list-params
# ~/ARM/avh-linux-x86/bin/FVP_MPS2_Cortex-M4 \
#     -C armcortexm4ct.semihosting-enable=0 \
#     -C fvp_mps2.UART0.out_file='-' \
#     -q \
#     -a "$@"


# Fast Models Fixed Virtual Platforms in Arm Development
# Studio Reference Guide
# Document ID: 110379_1129_00_en
# Issue 00
# MPS2
# 5. MPS2
# The Microcontroller Prototyping System 2 (MPS2) Fixed Virtual Platforms (FVPs) implement
# a subset of the functionality of the Arm Versatile Express V2M-MPS2 and V2M-MPS2+
# motherboard hardware.

# DAI0386C_cortex_m4_on_v2m_mps2 for the cortex m4

# Table 5-1: Memory map for models without the Armv8-M additions

# Description                                                                              | Modeled | Address range |
# ====================================================================================================================
#  Ethernet. Through ahb_to_extmem16. Oﬀset 0x0 to 0x0FE for CSRs,                         | Partial | 0xA0000000 to
#  0x100 to 0x1FE for FIFO.                                                                |         | 0xA000FFFF
# --------------------------------------------------------------------------------------------------------------------
#  PSRAM (16 MB)                                                                           | Yes     | 0x60000000 to
#                                                                                          |           0x60FFFFFF
# --------------------------------------------------------------------------------------------------------------------
#  VGA Image (512x128) (AHB)                                                               | Yes     | 0x41100000 to
#                                                                                          |         | 0x4110FFFF
# --------------------------------------------------------------------------------------------------------------------
#  VGA Console (AHB)                                                                       | Yes     | 0x41000000 to
#                                                                                          |         | 0x4100FFFF
# --------------------------------------------------------------------------------------------------------------------
#  Block RAM (boot time). Reserved 64 KB, 16 KB implemented. This memory Issue             | Yes     | 0x40200000 to
#  wrapped through the region.                                                             |         | 0x402FFFFF
# --------------------------------------------------------------------------------------------------------------------
#  Reserved                                                                                | N/A     | 0x40030000 to
#                                                                                          |         | 0x401FFFFF
# --------------------------------------------------------------------------------------------------------------------
#  SCC register                                                                            | Yes     | 0x4002F000 to
#                                                                                          |         | 0x4002FFFF
# --------------------------------------------------------------------------------------------------------------------
#  Reserved                                                                                | N/A     | 0x40029000 to
#                                                                                          |         | 0x4002EFFF
# --------------------------------------------------------------------------------------------------------------------
#  FPGA System Control & I/O, APB                                                          | Yes     | 0x40028000 to
#                                                                                          |         | 0x40028FFF
# --------------------------------------------------------------------------------------------------------------------
#  Reserved                                                                                | N/A     | 0x40025000 to
#                                                                                          |         | 0x40027FFF
# --------------------------------------------------------------------------------------------------------------------
#  Audio I2S, APB                                                                          | Partial | 0x40024000 to
#                                                                                          |         | 0x40024FFF
# --------------------------------------------------------------------------------------------------------------------
#  SBCon (Audio Conﬁguration), APB                                                         | Yes     | 0x40023000 to
#                                                                                          |         | 0x40023FFF
# --------------------------------------------------------------------------------------------------------------------
#  SBCon (Touch for LCD module), APB                                                       | Partial | 0x40022000 to
#                                                                                          |         | 0x40022FFF
# --------------------------------------------------------------------------------------------------------------------
#  PL022 (SPI for LCD module), APB                                                         | Partial | 0x40021000 to
#                                                                                          |         | 0x40021FFF
# --------------------------------------------------------------------------------------------------------------------
#  PL022 (SPI), APB                                                                        | Yes     | 0x40020000 to
#                                                                                          |         | 0x40020000
# --------------------------------------------------------------------------------------------------------------------
#  CMSDK system controller                                                                 | Yes     | 0x4001F000 to
#                                                                                          |         | 0x4001FFFF
# --------------------------------------------------------------------------------------------------------------------
#  Reserved for extra GPIO & other AHB peripherals                                         | N/A     | 0x40012000 to
#                                                                                          |         | 0x4001EFFF
# --------------------------------------------------------------------------------------------------------------------
#  CMSDK AHB GPIO #1                                                                       | N/A     | 0x40011000 to
#                                                                                          |         | 0x40011FFF
# --------------------------------------------------------------------------------------------------------------------
#  CMSDK AHB GPIO #0                                                                       | N/A     | 0x40010000 to
#                                                                                          |         | 0x40010FFF
# --------------------------------------------------------------------------------------------------------------------
#  CMSDK APB subsystem                                                                     | N/A     | 0x40000000 to
#  (note per Figure 3-2 of AN386 this contains Timers and UART)                            |         | 0x4000FFFF
# --------------------------------------------------------------------------------------------------------------------
#  Reserved                                                                                | N/A     | 0x20800000 to
#                                                                                          |         | 0x20FFFFFF
# --------------------------------------------------------------------------------------------------------------------
#  ZBTSRAM 2 & 3 (2x32-bit). Reserved 8 MB, 4 MB available.                                | N/A     | 0x20000000 to
#  The two SRAM blocks are interleaved.                                                    |         | 0x207FFFFF
# --------------------------------------------------------------------------------------------------------------------
#  Reserved                                                                                | N/A     | 0x01010000 to
#                                                                                          |         | 0x1FFFFFFF
# --------------------------------------------------------------------------------------------------------------------
#  Reserved                                                                                | N/A     | 0x00800000 to
#                                                                                          |         | 0x00FFFFFF
# --------------------------------------------------------------------------------------------------------------------
# ZBTSRAM 1 (64-bit). Wrapped. Only 4 MB ZBTSRAM ﬁtted.                                    | N/A     | 0x00400000 to
#                                                                                          |         | 0x007FFFFF
# --------------------------------------------------------------------------------------------------------------------
#  ZBTSRAM 1 (64-bit)                                                                      | N/A     | 0x00004000 to
#                                                                                          |         | 0x003FFFFF
# --------------------------------------------------------------------------------------------------------------------
# Mappable memory. When zbt_boot_ctrl = 0, ZBTSRAM 1 is mapped to this region.             | N/A     | 0x00000000 to
# Otherwise, Remap_ctrl = 0 maps Block RAM and Remap_ctrl = 1 maps ZBTSRAM 1.              |         | 0x00003FFF
# --------------------------------------------------------------------------------------------------------------------



# Table 5-3: Interrupt assignments
# | Number | Interrupt
# ======================================================================================================================
#  NMI     | Watchdog.
#  0       | UART 0 receive interrupt.
#  1       | UART 0 transmit interrupt.
#  2       | UART 1 receive interrupt.
#  3       | UART 1 transmit interrupt.
#  4       | UART 2 receive interrupt.
#  5       | UART 2 transmit interrupt.
#  6       | GPIO 0, 2 combined interrupt.
#  7       | GPIO 1, 3 combined interrupt.
#  8       | Timer 0.
#  9       | Timer 1.
#  10      | Dual Timer.
#  11      | SPI #1 (LCD). The LCD had shared SPI #0 and SPI #1.
#  12      | UART overﬂow (0, 1, 2).
#  13      | Ethernet.
#  14      | Audio I2S.
#  15      | Touch screen.
#  16-31   | GPIO 0 individual interrupts.
#  32-47   | GPIO 1 individual interrupts. Armv8-M additions.
#  48      | SPI #0. Armv8-M addition.
#  49      | Reserved.
#  50      | TRNG (Secure). Armv8-M addition.
#  51      | Unique ID and Secure storage (Secure). Armv8-M addition.
#  52      | DMA controller #0.
#  53      | DMA controller #1.
#  54      | SecureErrorIRQ. Armv8-M addition. Detection of Non-secure access to Secure address spaces, including other bus masters.
#          | Generated by Memory Gating unit, Peripheral Gating units, and bus gasket for legacy bus masters.

# The Ethernet module in the model is a LAN91C111. The hardware provides a LAN9220.
\



# 0x40004000-0x40004FFF UART0 per AN386
