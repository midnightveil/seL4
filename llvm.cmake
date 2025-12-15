#
# Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: GPL-2.0-only
#

cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_SYSTEM_NAME Generic)
# For a generic system this is unused, so define it to something that will be
# obvious if someone accidentally uses it
set(CMAKE_SYSTEM_PROCESSOR seL4CPU)

set(LLVM_TOOLCHAIN ON)

# When this file is passed to configure_file in cmake, these variables get set to
# the kernel platform configuration.
set(sel4_arch @KernelSel4Arch@)
set(arch @KernelArch@)
set(mode @KernelWordSize@)
set(cross_prefix @cross_prefix@)

if("${TRIPLE}" STREQUAL "")
    # https://doc.rust-lang.org/nightly/rustc/platform-support/thumbv7em-none-eabi.html
    if("${arch}" STREQUAL "Armv7-M")
        set(TRIPLE "thumbv7em-none-eabi")
    elseif("${arch}" STREQUAL "@KernelArch@")
        message(FATAL_ERROR "pass LLVM_TOOLCHAIN=ON to init.sh not the toolchain directly")
    else()
        message(FATAL_ERROR "unknown LLVM target for ${arch}")
    endif()
endif()

set(CROSS_COMPILER_PREFIX ${TRIPLE}-)

set(CMAKE_ASM_COMPILER "clang")
set(CMAKE_ASM_COMPILER_ID Clang)
set(CMAKE_ASM_COMPILER_TARGET ${TRIPLE})

set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_COMPILER_ID Clang)
set(CMAKE_C_COMPILER_TARGET ${TRIPLE})

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_COMPILER_ID Clang)
set(CMAKE_CXX_COMPILER_TARGET ${TRIPLE})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

mark_as_advanced(FORCE CMAKE_TOOLCHAIN_FILE)

find_program(CCACHE ccache)
if(NOT ("${CCACHE}" STREQUAL CCACHE-NOTFOUND))
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
endif()
mark_as_advanced(CCACHE)
