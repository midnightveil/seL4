#include <api/failures.h>
#include <arch/kernel/mpu.h>
#include <arch/machine.h>
#include <kernel/faulthandler.h>
#include <kernel/thread.h>
#include <model/statedata.h>

void handleMemManageFault(word_t CFSR)
{
    uint8_t MMFSR = CFSR_BFSR_EXTRACT(CFSR);
    /* If MMFSR.MMFARVALID is not set, the value is architecturally ᴜɴᴋɴᴏᴡɴ */
    uint32_t MMFAR = 0;
    if (MMFSR & MMFSR_MMFARVALID) {
        MMFAR = *SCB_MMFAR;
    }

    current_fault = seL4_Fault_MemManageFault_new(MMFAR, MMFSR);

    handleFault(NODE_STATE(ksCurThread));
    schedule();
    activateThread();
}

void handleBusFault(word_t CFSR)
{
    uint8_t BFSR = CFSR_BFSR_EXTRACT(CFSR);

// xxx: derived
// https://github.com/oxidecomputer/hubris/blob/17da6ea3010ab51fe0b629ea5b425b8cac4adef4/sys/kern/src/arch/arm_m.rs#L1693-L1722
    *SCB_SHCSR &= ~(0b1111 << 12);
    dsb();
    isb();

    uint32_t BFAR = 0;
    /* If BFSR.BFARVALID is not set, the value is architecturally ᴜɴᴋɴᴏᴡɴ */
    if (BFSR & BFSR_BFARVALID) {
        BFAR = *SCB_BFAR;
    }

    // TODO: horrible hack
    *SCB_CFSR = CFSR;
    dsb();
    isb();

    current_fault = seL4_Fault_BusFault_new(BFAR, BFSR);

    handleFault(NODE_STATE(ksCurThread));
    schedule();
    activateThread();
}
