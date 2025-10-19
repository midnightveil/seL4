
#pragma once
#include <sel4/config.h>
#include <sel4/macros.h>

#ifndef __ASSEMBLER__

/* format of an unknown syscall message */
typedef enum {
    seL4_UnknownSyscall_R0,
    seL4_UnknownSyscall_R1,
    seL4_UnknownSyscall_R2,
    seL4_UnknownSyscall_R3,
    seL4_UnknownSyscall_R4,
    seL4_UnknownSyscall_R5,
    seL4_UnknownSyscall_R6,
    seL4_UnknownSyscall_R7,
    seL4_UnknownSyscall_FaultIP,
    seL4_UnknownSyscall_SP,
    seL4_UnknownSyscall_LR,
    // TODO
    seL4_UnknownSyscall_CPSR,
    seL4_UnknownSyscall_Syscall,
    /* length of an unknown syscall message */
    seL4_UnknownSyscall_Length,
    SEL4_FORCE_LONG_ENUM(seL4_UnknownSyscall_Msg),
} seL4_UnknownSyscall_Msg;

/* format of a user exception message */
typedef enum {
    seL4_UserException_FaultIP,
    seL4_UserException_SP,
    seL4_UserException_CPSR,
    seL4_UserException_Number,
    seL4_UserException_Code,
    /* length of a user exception */
    seL4_UserException_Length,
    SEL4_FORCE_LONG_ENUM(seL4_UserException_Msg),
} seL4_UserException_Msg;

#endif
