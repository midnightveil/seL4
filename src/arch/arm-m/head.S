/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <machine/assembler.h>

.section .boot.text, "ax"
BEGIN_FUNC(_start)
    b _start
END_FUNC(_start)
