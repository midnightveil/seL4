/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <api/debug.h>
#include <arch/machine.h>
#include <util.h>

#ifdef CONFIG_DEBUG_BUILD
static inline void debug_printExceptionInformation(void)
{
    word_t xPSR = MRS("xPSR");
    printf("xPSR value was 0x%"SEL4_PRIx_word"\n", xPSR);
    /* B1.4.2 Interrupt Program Status Register */
    word_t IPSR = xPSR & xPSR_IPSR;
    /* B3.2.15 Configurable Fault Sytatus Register, page B3-609 */
    word_t CFSR = *SCB_CFSR;
    printf("CFSR value was 0x%"SEL4_PRIx_word"\n", CFSR);

    /**
     * Also reference B1.5.14 "List of Armv7-M faults" which includes more
     * detailed fault subtypes.
     * Also see AN209 on Fault exceptions: https://www.keil.com/appnotes/files/apnt209.pdf.
     **/

    /* see Table B1-4 */
    switch (IPSR) {
    case 0: /* no exception */
        printf("no exception being processed\n");
        break;

    case 1:
        printf("Reset exception\n");
        break;

    case 2:
        printf("NMI exception\n");
        break;

    case 3:
        printf("HardFault exception\n");
        break;

    case 4:
        printf("MemManage exception\n");
        break;

    case 5:
        printf("BusFault exception\n");
        break;

    case 6:
        printf("UsageFault exception\n");
        break;

    case 11:
        printf("SVCall exception\n");
        break;

    case 12:
        printf("DebugMonitor exception\n");
        break;

    case 14:
        printf("PendSV exception\n");
        break;

    case 15:
        printf("SysTick exception\n");
        break;

    default:
        printf("unknown exception %"SEL4_PRIx_word"\n", IPSR);
        break;
    }
}
#endif

/** DONT_TRANSLATE */
void NORETURN NO_INLINE VISIBLE halt(void)
{
    /* halt is actually, idle thread without the interrupts */


    /* B5.2.1 CPS in DDI 0403E.e, p. B5-672
       Change Processor State, sets PRIMASK=1 and FAULTMASK=1.
    */
    asm volatile("cpsid if");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
    debug_printExceptionInformation();
    Arch_userStackTrace(NODE_STATE(ksCurThread));
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
