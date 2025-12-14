/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

// from arm32 one.
// TODO: I'm so confused what these all mean.

#include <assert.h>
#include <arch/machine/registerset.h>

/** msgRegisters, used as syscall args.
 *  We use the kernel-saved ones in case it's possible to omit duplicating
 *  the hardware stacked registers in future (see registerset.h).
 *
 *  R4, R5, R6 are already taken by cap/badge/msgInfo registers
 **/
const register_t msgRegisters[] = {
    R7, R8, R9, R10
};
compile_assert(
    consistent_message_registers,
    sizeof(msgRegisters) / sizeof(msgRegisters[0]) == n_msgRegisters
);

// FIXME: I don't understand frame vs gpRegisters. This is copied from arm32.

const register_t frameRegisters[] = {
    FaultIP, SP_process, xPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    sizeof(frameRegisters) / sizeof(frameRegisters[0]) == n_frameRegisters
);

const register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, LR
};
compile_assert(
    consistent_gp_registers,
    sizeof(gpRegisters) / sizeof(gpRegisters[0]) == n_gpRegisters
);


