#pragma once

#include <arch/types.h>
#include <sel4/sel4_arch/constants.h>

enum _register {
    pc = 0, R15 = 0,
    LR = 1, R14 = 1,
    psp = 2, R13 = 2, SP = 2,

    R12 = 3,
    R11 = 4,
    R10 = 5,
    R9 = 6,
    R8 = 7,
#ifdef CONFIG_KERNEL_MCS
    nbsendRecvDest = 7,
#endif

    R7 = 8,
    R6 = 9,
#ifdef CONFIG_KERNEL_MCS
    replyRegister = 9,
#endif
    R5 = 10,
    R4 = 11,
    R3 = 12,
    R2 = 13,
    R1 = 14, msgInfoRegister = 1,
    R0 = 15, capRegister = 15, badgeRegister = 0,

   /* End of GP registers, the following are additional kernel-saved state. */

    NextIP = 15,
    FaultIP = 16,

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
    n_syscallMessage = 12,
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
    [seL4_UserException_SP] = SP\
    /* [seL4_UserException_CPSR] = CPSR */\
 }

#define SYSCALL_MESSAGE \
{\
    [seL4_UnknownSyscall_R0] = R0,\
    [seL4_UnknownSyscall_R1] = R1,\
    [seL4_UnknownSyscall_R2] = R2,\
    [seL4_UnknownSyscall_R3] = R3,\
    [seL4_UnknownSyscall_R4] = R4,\
    [seL4_UnknownSyscall_R5] = R5,\
    [seL4_UnknownSyscall_R6] = R6,\
    [seL4_UnknownSyscall_R7] = R7,\
    [seL4_UnknownSyscall_FaultIP] = FaultIP,\
    [seL4_UnknownSyscall_SP] = SP,\
    [seL4_UnknownSyscall_LR] = LR \
    /* [seL4_UnknownSyscall_CPSR] = CPSR */ \
}

#define TIMEOUT_REPLY_MESSAGE \
{\
    [seL4_TimeoutReply_FaultIP] = FaultIP,\
    [seL4_TimeoutReply_SP] = SP, \
    /* [seL4_TimeoutReply_CPSR] = CPSR, */ \
    [seL4_TimeoutReply_R0] = R0,\
    [seL4_TimeoutReply_R1] = R1,\
    [seL4_TimeoutReply_R8] = R8,\
    [seL4_TimeoutReply_R9] = R9,\
    [seL4_TimeoutReply_R10] = R10,\
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
    // context->registers[CPSR] = CPSR_USER;
#ifdef CONFIG_HAVE_FPU
    context->fpuState.fpexc = BIT(FPEXC_EN_BIT);
#endif
#ifdef ARM_BASE_CP14_SAVE_AND_RESTORE
    Arch_initBreakpointContext(context);
#endif
}
