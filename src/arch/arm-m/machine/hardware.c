#include <arch/machine.h>

#include <types.h>
#include <machine/registerset.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    return getRegister(thread, FaultIP);
}

void setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NextIP, v);
}

void setNextSP(tcb_t *thread, word_t v)
{
    /**
     * We assume that in this situation when we set a SP explicitly we are
     * setting the top of the stack (this is the only user of this function
     * at the moment). Hence, since Cortex-M does hardware (un)stacking of
     * the exception frame, to simulate a thread that is ready for unstacking
     * (i.e. in the kernel) we must 'push' the stack pointer downwards by
     * the size of the exception stack frame.
     *
     * We have disabled the FPU, so framesize = 0x20, which is the value of PT_R4.
     **/
    setRegister(thread, SP_process, v - PT_R4);
}
