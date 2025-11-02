/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <machine/assembler.h>

/* https://community.arm.com/arm-community-blogs/b/architectures-and-processors-blog/posts/writing-your-own-startup-code-for-cortex-m */

.syntax unified
.thumb

BEGIN_FUNC(arm_nmi_handler)
    b halt
END_FUNC(arm_nmi_handler)

BEGIN_FUNC(arm_hard_fault_handler)
    b halt
END_FUNC(arm_hard_fault_handler)

BEGIN_FUNC(arm_memory_management_fault)
    b halt
END_FUNC(arm_memory_management_fault)

BEGIN_FUNC(arm_bus_fault)
    b halt
END_FUNC(arm_bus_fault)

BEGIN_FUNC(arm_usage_fault)
    b halt
END_FUNC(arm_usage_fault)

