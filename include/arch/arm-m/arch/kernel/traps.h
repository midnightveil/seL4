#pragma once

// taken from arm32 traps

/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <arch/machine.h>
#include <config.h>
#include <machine.h>
#include <util.h>

extern word_t arm_vector_table[];

static inline void arch_c_entry_hook(void)
{
    // arm_save_thread_id(NODE_STATE(ksCurThread));
}

static inline void arch_c_exit_hook(void)
{
    // arm_load_thread_id(NODE_STATE(ksCurThread));


    /** Page B1-540 tells us that if we are not in Handler mode, then loading the
     *  EXC_RETURN value produces either a MemManage or INVState UsageFault
     *  Don't let this happen; check that the current exception number is non-zero
     *   => non-Thread mode.
     **/
    assert(MRS("IPSR") != 0);
}

void c_handle_syscall(word_t cptr, word_t msgInfo, syscall_t syscall)
VISIBLE SECTION(".text.vectors");

void c_handle_interrupt(void)
VISIBLE SECTION(".text.vectors");

void c_handle_undefined_instruction(void)
VISIBLE SECTION(".text.vectors");

void c_handle_data_fault(void)
VISIBLE SECTION(".text.vectors");

void c_handle_instruction_fault(void)
VISIBLE SECTION(".text.vectors");

void restore_user_context(void)
VISIBLE NORETURN SECTION(".text.traps");
