#pragma once

#include <api/failures.h>
#include <arch/types.h>
#include <basic_types.h>
#include <object/structures.h>
#include <util.h>

/* TODO: ??? best spot, was in vspace.h */
word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread);
exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap);
#ifdef CONFIG_PRINTING
void Arch_userStackTrace(tcb_t *tptr);
#endif


/* XXX: directly arm thread.h **/


#pragma once

static inline word_t sanitiseRegister(register_t reg, word_t v, bool_t archInfo)
{
    /* TODO */
    return v;
}

static inline bool_t PURE Arch_getSanitiseRegisterInfo(tcb_t *thread)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    return (thread->tcbArch.tcbVCPU != NULL);
#else
    return 0;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}
