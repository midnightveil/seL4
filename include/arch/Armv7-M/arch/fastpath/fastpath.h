/**
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 **/

#pragma once

#include <arch/api/syscall.h>
#include <linker.h>

void slowpath(syscall_t syscall);
