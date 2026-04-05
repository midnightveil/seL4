/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <api/syscall.h>
#include <arch/fastpath/fastpath.h>
#include <arch/kernel/mpu.h>
#include <arch/kernel/traps.h>
#include <arch/machine.h>
#include <arch/machine/registerset.h>
#include <arch/types.h>
#include <benchmark/benchmark_track.h>
#include <config.h>
#include <kernel/stack.h>
#include <kernel/traps.h>
#include <linker.h>
#include <model/statedata.h>
#include <util.h>

extern char arm_Reset_exception[1] BOOT_CODE;
/* This handles *multiple* exceptions, to minimise program text. */
extern char arm_handle_exception[1];
extern char arm_SVCall_exception[1];
extern char arm_PendSV_exception[1];

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
    [ 2] = (word_t)&arm_handle_exception, /* NMI */
    [ 3] = (word_t)&arm_handle_exception, /* HardFault */
    [ 4] = (word_t)&arm_handle_exception, /* MemManage */
    [ 5] = (word_t)&arm_handle_exception, /* BusFault */
    [ 6] = (word_t)&arm_handle_exception, /* UsageFault */
#ifdef CONFIG_ARCH_ARMV8M
    [ 7] = (word_t)&arm_handle_exception, /* SecureFault */
#else
    [ 7] = 0 /* Reserved */,
#endif
    [ 8] = 0 /* Reserved */,
    [ 9] = 0 /* Reserved */,
    [10] = 0 /* Reserved */,
    [11] = (word_t)&arm_SVCall_exception,
    [12] = (word_t)&arm_handle_exception, /* DebugMonitor */
    [13] = 0 /* Reserved */,
    [14] = (word_t)&arm_PendSV_exception,
    [15] = (word_t)&arm_handle_exception, /* SysTick */
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

void c_handle_syscall(word_t cptr, word_t msgInfo, syscall_t syscall)
{
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, msgInfo, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* TRACK_KERNEL_ENTRIES */

    slowpath(syscall);

    c_exit_hook();
}

void c_handle_exception(void)
{
    c_entry_hook();

    word_t IPSR = MRS("IPSR");
    /* B3.9 of Armv8-M ARM DDI 0553B.y
     *  TODO: Armv7-M
     * A non-interrupt exception is in the range [0x1, 0xf].
     */
    uint8_t exception_number = IPSR & 0xf;

    // TODO: check exc_return to see if we can from handle mode, i.e. nested, then goto kernel_abort
    assert(NODE_STATE(ksCurThread) != NULL);

    word_t CFSR = *SCB_CFSR;

    switch (exception_number) {
    case 4: /* MemManage */ {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_MemoryFault;
        ksKernelEntry.word = exception_number;
#endif
        handleMemManageFault(CFSR);
        break;
    }

    case 5: /* BusFault */ {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_MemoryFault;
        ksKernelEntry.word = exception_number;
#endif
        handleBusFault(CFSR);
        break;
    }

    case 6: /* UsageFault */ {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UserLevelFault;
        ksKernelEntry.word = exception_number;
#endif
        handleUserLevelFault(CFSR_UFSR_EXTRACT(CFSR), 0);
        break;
    }

    // These can happen, as they are routed here, but they should not happen
    case 1: /* Reset */
    case 2: /* NMI */
    case 3: /* HardFault */
#ifdef CONFIG_ARCH_ARMV8M
    case 7: /* SecureFault */
#endif
    case 12: /* DebugMonitor */
        goto kernel_abort;

    // Other numbers should be impossible to go this path.
    default:
        assert(!"unreachable");
        break;
    }

    c_exit_hook();

    return;

kernel_abort:
    printf("KERNEL ABORT: exception %"SEL4_PRIu_word"\n", (word_t)exception_number);
    halt();
}

void c_handle_interrupt(void)
{
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_INDEX();
#endif /* TRACK_KERNEL_ENTRIES */

    handleInterruptEntry();

    c_exit_hook();
}
