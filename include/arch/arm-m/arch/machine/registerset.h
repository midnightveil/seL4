#pragma once

#include <arch/types.h>
#include <sel4/sel4_arch/constants.h>

/**
 * restore_user_context() uses the layout of the 'basic frame' (Figure B1-3),
 * with the additional kernel-saved registers (R4-R11) afterwards.
 *
 * Also look at the PopStack() pseudocode on page B1-542.
 **/
enum _register {
    // TODO: I don't think we actually need this.


    /** 'Basic frame', pushed/popped by hardware **/
    /* 0x00 */ R0 = 0,
    capRegister = R0,
    badgeRegister = R0,
    /* 0x04 */ R1 = 1,
    msgInfoRegister = R1,
    /* 0x08 */ R2 = 2,
    /* 0x0C */ R3 = 3,
    /* 0x10 */ R12 = 4,
    /* 0x14 */ LR = 5, /* R14 */
    /* 0x18 */ PC = 6, /* R15 */
    NextIP = PC,
    /* 0x1C */ xPSR = 7,

    // TODO: The 'Extended frame' FPU registers would continue from here.

    /** Additional kernel-saved GP registers **/
    R4 = 8,
    R5 = 9,
    R6 = 10,
    R7 = 11,
    R8 = 12,
    R9 = 13,
    R10 = 14,
    R11 = 15,
    exc_return = 16,

    SP_process = 17, /* R13 */

    /** Extra kernel-saved state **/
    FaultIP = 18,

    n_contextRegisters
};

typedef struct user_context {
    word_t registers[n_contextRegisters];
} user_context_t;

typedef word_t register_t;

enum messageSizes {
    n_msgRegisters = seL4_FastMessageRegisters,
    n_frameRegisters = 10,
    n_gpRegisters = 7,
    n_exceptionMessage = 3,
    n_syscallMessage = 8,
#ifdef CONFIG_KERNEL_MCS
    n_timeoutMessage = 17,
#endif
};

// TODO....
extern const register_t msgRegisters[];
extern const register_t frameRegisters[];
extern const register_t gpRegisters[];

#define EXCEPTION_MESSAGE \
 {\
    [seL4_UserException_FaultIP] = FaultIP,\
    [seL4_UserException_SP] = SP_process,\
    [seL4_UserException_xPSR] = xPSR \
 }

#define SYSCALL_MESSAGE \
{\
    [seL4_UnknownSyscall_R0] = R0,\
    [seL4_UnknownSyscall_R1] = R1,\
    [seL4_UnknownSyscall_R2] = R2,\
    [seL4_UnknownSyscall_R3] = R3,\
    [seL4_UnknownSyscall_FaultIP] = FaultIP,\
    [seL4_UnknownSyscall_SP] = SP_process,\
    [seL4_UnknownSyscall_LR] = LR,\
    [seL4_UnknownSyscall_xPSR] = xPSR \
}

#define TIMEOUT_REPLY_MESSAGE \
{\
    [seL4_TimeoutReply_FaultIP] = FaultIP,\
    [seL4_TimeoutReply_SP] = SP_process, \
    [seL4_TimeoutReply_R0] = R0,\
    [seL4_TimeoutReply_R1] = R1,\
    [seL4_TimeoutReply_R2] = R2,\
    [seL4_TimeoutReply_R3] = R3,\
    [seL4_TimeoutReply_R12] = R12,\
    [seL4_TimeoutReply_xPSR] = xPSR, \
    [seL4_TimeoutReply_R11] = R11,\
    [seL4_TimeoutReply_R12] = R12,\
    [seL4_TimeoutReply_R2] = R2,\
    [seL4_TimeoutReply_R3] = R3,\
    [seL4_TimeoutReply_R4] = R4,\
    [seL4_TimeoutReply_R5] = R5,\
    [seL4_TimeoutReply_R6] = R6,\
    [seL4_TimeoutReply_R7] = R7,\
    [seL4_TimeoutReply_R14] = R14,\
}

static inline void Arch_initContext(user_context_t *context)
{
    // TODO
    // context->registers[CPSR] = CPSR_USER;
}
