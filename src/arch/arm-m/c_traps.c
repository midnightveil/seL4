/*
 * Copyright 2025, Julia V
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <assert.h>
#include <arch/kernel/traps.h>
#include <arch/machine.h>
#include <arch/machine/registerset.h>
#include <arch/types.h>
#include <config.h>
#include <model/statedata.h>
#include <util.h>

// #define EXC_RETURN_CONST 0xFFFFFFFD

// TODO: we don't need this? I think?

/** DONT_TRANSLATE */
void VISIBLE NORETURN restore_user_context(void)
{
    c_exit_hook();

    /* Look at PopStack() pseudocode on page B1-542. */

    word_t *user_context_regs = NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers;

    compile_assert(hardware_restored_frame_size, 0x20 == sizeof(word_t) * (xPSR - R0 + 1));
    compile_assert(r4_first_after_hardware_restore, 0x20 == sizeof(word_t) * (R4));
    compile_assert(exc_return_after_r11, R11 + 1 == exc_return);

    /* Per the PopStack() pseudocode, set PSP/frameptr to the exception frame,
       which was saved on exception/interrupt entry.
     */
    MSR("PSP", (word_t)user_context_regs[PSP]);

    /** Page B1-540 tells us that if we are not in Handler mode, then loading the
     *  EXC_RETURN value produces either a MemManage or INVState UsageFault
     *  Don't let this happen; check that the current exception number is non-zero
     *   => non-Thread mode.
     **/

    assert(MRS("IPSR") != 0);

    asm volatile(
        /**
         * restore the callee-saved registers, which includes the EXC_RETURN value
         * per B1.5.8, an exception return occurs when a LDM loads an EXC_RETURN
         * value into the PC.
         **/
        "ldm %[kernel_restored_regs], {r4-r11, pc}       \n"
        : /* no outputs */
        : [kernel_restored_regs] "r"(&user_context_regs[R4])
        : "memory"
    );

    UNREACHABLE();
}
