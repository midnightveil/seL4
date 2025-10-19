#pragma once

#include <api/failures.h>
#include <arch/types.h>
#include <basic_types.h>
#include <object/structures.h>
#include <util.h>

/* TODO: ??? best spot, was in vspace.h */
word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread);
exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap);



/* XXX: directly arm thread.h **/


#pragma once

static inline word_t sanitiseRegister(register_t reg, word_t v, bool_t archInfo)
{
    /* TODO */
    #define CPSR 0xfffffff
    #define CPSR_USER 0xfffffff
    if (reg == CPSR) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* on aarch32 archInfo means 'has VCPU', see Arch_getSanitiseRegisterInfo below */
        if (archInfo) {
            switch (v & 0x1f) {
            case PMODE_USER:
            case PMODE_FIQ:
            case PMODE_IRQ:
            case PMODE_SUPERVISOR:
            case PMODE_ABORT:
            case PMODE_UNDEFINED:
            case PMODE_SYSTEM:
                return v;
            case PMODE_HYPERVISOR:
            default:
                /* For backwards compatibility, Invalid modes revert to USER mode */
                break;
            }
        }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

        return (v & 0xf8000000) | CPSR_USER;
    } else {
        return v;
    }
}

static inline bool_t PURE Arch_getSanitiseRegisterInfo(tcb_t *thread)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    return (thread->tcbArch.tcbVCPU != NULL);
#else
    return 0;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}
