/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <object.h>
#include <machine.h>
#include <arch/model/statedata.h>
#include <arch/kernel/thread.h>
#include <linker.h>

void Arch_switchToThread(tcb_t *tcb)
{
    // setVMRoot(tcb);
    // clearExMonitor();
}

BOOT_CODE void Arch_configureIdleThread(tcb_t *tcb)
{
    // setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    // setRegister(tcb, NextIP, (word_t)&idle_thread);
}

void Arch_switchToIdleThread(void)
{
    /* Force the idle thread to run on kernel page table */
    // setVMRoot(NODE_STATE(ksIdleThread));
}

void Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}



word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    // TODO
    return NULL;
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    // TODO
    return EXCEPTION_SYSCALL_ERROR;
}
