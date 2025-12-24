/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <api/syscall.h>
#include <arch/fastpath/fastpath.h>
#include <arch/kernel/traps.h>
#include <arch/machine.h>
#include <arch/machine/registerset.h>
#include <arch/types.h>
#include <config.h>
#include <kernel/stack.h>
#include <kernel/traps.h>
#include <linker.h>
#include <model/statedata.h>
#include <util.h>

extern char arm_Reset_exception[1] BOOT_CODE;
extern char arm_NMI_exception[1];
extern char arm_HardFault_exception[1];
extern char arm_MemManage_exception[1];
extern char arm_BusFault_exception[1];
extern char arm_UsageFault_exception[1];
extern char arm_SVCall_exception[1];
extern char arm_DebugMonitor_exception[1];
extern char arm_DebugMonitor_exception[1];
extern char arm_PendSV_exception[1];
extern char arm_SysTick_exception[1];

/**
 * Read the Armv7-M exception model, B1.5 of Armv7-M ARM DDI 0403E.e.
 * The exception numbers and ordering of the vector table is defined in
 * Tables B1-4 and B1-5.
 *
 * > The Vector table must be naturally aligned to a power of two whose alignment
 * > value is greater than or equal to (Number of Exceptions supported x 4),
 * > with a minimum alignment of 128 bytes.
 * > All other entries must have bit[0] set to 1, because this bit
 * > defines the EPSR.T bit on exception entry.
 * TODO
 **/
word_t arm_vector_table[16] ALIGN(128) SECTION(".vectors") = {
    [ 0] = ((word_t)&kernel_stack_alloc) + BIT(CONFIG_KERNEL_STACK_BITS),
    [ 1] = (word_t)&arm_Reset_exception,
    [ 2] = (word_t)&arm_NMI_exception,
    [ 3] = (word_t)&arm_HardFault_exception,
    [ 4] = (word_t)&arm_MemManage_exception,
    [ 5] = (word_t)&arm_BusFault_exception,
    [ 6] = (word_t)&arm_UsageFault_exception,
    [ 7] = 0 /* Reserved */,
    [ 8] = 0 /* Reserved */,
    [ 9] = 0 /* Reserved */,
    [10] = 0 /* Reserved */,
    [11] = (word_t)&arm_SVCall_exception,
    [12] = (word_t)&arm_DebugMonitor_exception,
    [13] = 0 /* Reserved */,
    [14] = (word_t)&arm_PendSV_exception,
    [15] = (word_t)&arm_SysTick_exception,
};

void slowpath(syscall_t syscall)
{
    if (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } else {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }
}

/**
 * Unlike the rest of the architectures, we make all our functions return
 * to the assembly trap functions.
 **/

void VISIBLE c_handle_syscall(word_t cptr, word_t msgInfo, syscall_t syscall)
{
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, msgInfo, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);

    c_exit_hook();
}
