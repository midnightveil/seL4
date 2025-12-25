/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <arch/kernel/thread.h>
#include <arch/machine/registerset.h>
#include <arch/model/statedata.h>
#include <config.h>
#include <kernel/thread.h>
#include <linker.h>
#include <machine.h>
#include <object.h>

void Arch_switchToThread(tcb_t *tcb)
{
    // setVMRoot(tcb);
    // clearExMonitor();
}

BOOT_CODE void Arch_configureIdleThread(tcb_t *tcb)
{
    Arch_initContext(&tcb->tcbArch.tcbContext);

    // setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    /* Note: we set this to the bottom of the stack, because we are emulating
       an idle thread which has already *had* its registers stacked upon entry,
       and unstacking will increase the SP.
     */
    setRegister(tcb, SP_process, (word_t)&ksArmMIdleThreadStack);

    setRegister(tcb, R0, 0x1000);
    setRegister(tcb, R1, 0x1001);
    setRegister(tcb, R2, 0x1002);
    setRegister(tcb, R3, 0x1003);
    setRegister(tcb, R4, 0x1004);
    setRegister(tcb, R5, 0x1005);
    setRegister(tcb, R6, 0x1006);
    setRegister(tcb, R7, 0x1007);
    setRegister(tcb, R8, 0x1008);
    setRegister(tcb, R9, 0x1009);
    setRegister(tcb, R10, 0x1010);
    setRegister(tcb, R11, 0x1011);
    setRegister(tcb, R12, 0x1012);

    Arch_postModifyRegisters(tcb);
}

void Arch_switchToIdleThread(void)
{
    /* Force the idle thread to run on kernel page table */
    // setVMRoot(NODE_STATE(ksIdleThread));

    // TODO: MPU?
}

// TODO: why does this eeven exist?
void Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    fail("lookupIPCBuffer");
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    fail("checkValidIPCBuffer");
}
