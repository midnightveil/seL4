#pragma once

// TODO: what does PT mean?
/* Offsets within the user context for assembly, these need to match the order
 * in user_context_t below */
#define PT_R4 (8 * 4)
#define PT_NextIP (6 * 4)

#ifndef __ASSEMBLER__ /* C only definitions */

#include <util.h>
#include <arch/types.h>
#include <assert.h>
#include <arch/object/structures.h>
#include <object/structures.h>
#include <sel4/sel4_arch/constants.h>

/**
 * restore_user_context() uses the layout of the 'basic frame' (Figure B1-3),
 * with the additional kernel-saved registers (R4-R11) afterwards.
 *
 * XXX: It might be possible to get away with only the hardware-stacked registers,
 *      but it breaks a lot of everything if we have to read PSP to be
 *      able to do anything, so we increase the syscall overhead by forcing
 *      save/restore of the already saved values.
 *
 * Also look at the PopStack() pseudocode on page B1-542.
 **/
enum _register {
    /** 'Basic frame', pushed/popped by hardware **/
    /* 0x00 */ R0 = 0,
    /* 0x04 */ R1 = 1,
    /* 0x08 */ R2 = 2,
    /* 0x0C */ R3 = 3,
    /* 0x10 */ R12 = 4,
    /* 0x14 */ LR = 5, /* R14 */
    /* 0x18 */ PC = 6, /* R15 */
    NextIP = PC,
    /* 0x1C */ xPSR = 7,

    // The 'Extended frame' FPU registers would continue from here.

    /** Additional kernel-saved GP registers. The layout is important
     *  in c_traps. **/
    R4 = 8,
    capRegister = R4,
    badgeRegister = R4,
    R5 = 9,
    msgInfoRegister = R5,
    R6 = 10,
    R7 = 11,
    R8 = 12,
    R9 = 13,
    R10 = 14,
#ifdef CONFIG_KERNEL_MCS
    replyRegister = R10,
#endif
    /* used as the syscall number register */
    R11 = 15,
    SP_process = 16, /* R13 */
    EXC_RETURN = 17,

    /** Extra kernel-saved state **/
    FaultIP = 18,

    n_contextRegisters
};

typedef word_t register_t;

typedef struct user_context {
    register_t registers[n_contextRegisters];
} user_context_t;


unverified_compile_assert(registers_are_first_member_of_user_context,
                          OFFSETOF(user_context_t, registers) == 0);
#if 0
unverified_compile_assert(user_context_is_first_member_of_arch_tcb,
                          OFFSETOF(arch_tcb_t, user_context) == 0);
unverified_compile_assert(arch_tcb_is_first_member_of_tcb,
                          OFFSETOF(tcb_t, tcbArch) == 0);
#endif

compile_assert(r4_offset_correct, R4 * sizeof(register_t) == PT_R4);
compile_assert(nextip_offset_correct, NextIP * sizeof(register_t) == PT_NextIP);

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
    // TODO: why is secure
    // TODO: more on xPSR

#ifndef CONFIG_ARCH_ARMV8M
    #error need to figure out secure/non-secure
#endif

    context->registers[EXC_RETURN] = 0xFFFFFF00
                                   | /* S = secure   */ (1 ? BIT(6) : 0)
                                   // | /* S = non-secure   */ (0 ? BIT(6) : 0)
    // xxx: I don't understand what this means.  RWCGV on p121 says that if EXC_RETURN.S=1 and EXC_RETURN.DCRS=0 then we try the additional state, which we don't want.
                                   | /* DCRS = default rules followed   */ (1 ? BIT(5) : 0)
                                   | /* FType = standard */ (1 ? BIT(4) : 0)
                                   | /* Mode = thread    */ (1 ? BIT(3) : 0)
                                   | /* SPSEL = process  */ (1 ? BIT(2) : 0)
                                   | /* reserved         */ (0 ? BIT(1) : 0)
                                   | /* exception sec = 1*/ (1 ? BIT(0) : 0);
                                   // | /* exception sec = 1*/ (0 ? BIT(0) : 0);

    context->registers[xPSR] = BIT(24); // thumb mode
}

#endif /* !__ASSEMBLER__ */
