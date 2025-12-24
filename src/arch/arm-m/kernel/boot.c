#include <arch/kernel/boot.h>
#include <arch/kernel/traps.h>
#include <arch/machine/registerset.h>
#include <kernel/boot.h>
#include <kernel/thread.h>
#include <linker.h>
#include <machine.h>
#include <machine/io.h>
#include <machine/fpu.h>
#include <object/objecttype.h>
#include <util.h>

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_CODE static bool_t init_cpu(void)
{
    #if 0
        #define VERBOSE_CPU_PRINT(...) printf(__VA_ARGS__)
    #else
        #define VERBOSE_CPU_PRINT(...) ((void)(0))
    #endif

    /* At this point we're roughly assuming that the CPU is in reset.
       Armv7-M ARM DDI 0403E.e B1.5.5 "Reset behaviour" shows a set of registers
       that are set at system reset, and where possible we want to initialise
       or reset these values as desired.
     */

    /* We first assume that we are in "Thread Mode" and "Privileged execution".
       Reference Armv7-M ARM DDI 0403E.e B1.3.1 and B1.4.4.
       We can't check the thread mode, but we can check the privilege level
       and available stacks.
     */

    word_t CONTROL = MRS("CONTROL");
    if (CONTROL & CONTROL_nPRIV) {
        printf("Execution is unprivileged\n");
        return false;
    } else if (CONTROL & CONTROL_SPSEL) {
        /* If using our bootcode, the initial stack the vector table starts with
           is the SP_main per B1.5.5 */
        printf("ERROR: Stack pointer is SP_process, expected SP_main\n");
        return false;
    }

    /* Read the Program Status Register xPSR, ref Armv7-M ARM DDI 0403E.e B1.4.2
       (a combination of APSR, IPSR, and EPSR).
       Note that the EPSR is always read-as-zero when read via an MRS.
     */
    word_t xPSR = MRS("xPSR");
    VERBOSE_CPU_PRINT("xPSR: %"SEL4_PRIx_word"\n", xPSR);
    if ((xPSR & xPSR_IPSR) != 0) {
        /* The IPSR contains the value 0 in Thread Mode, or the exception number
           in Handler mode. */
        printf("ERROR: In Handler mode with Exception Number %"SEL4_PRIx_word"\n", xPSR & xPSR_IPSR);
        return false;
    }

    /* Clear priority and fault masks */
    MSR("PRIMASK", 0);
    MSR("FAULTMASK", 0);

    bool_t haveHWFPU = fp_HWCapTest();
    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    if (haveHWFPU) {
        // TODO: Support CONFIG_HAVE_FPU. Follow "if HaveFPExt() then"
        printf("Platform has an FPU, disabling\n");
        fp_HWDisable();
    }

    /**
     * Now we want to initialise any system configuration registers in the
     * System Control Space (SCS) as appropriate, per B3.2.
     *
     * First up is the CPUID register (§B3.2.3). This is RO, so there is nothing to do.
     * It is always implemented and has no usage constraints.
     */
    VERBOSE_CPU_PRINT("CPUID: 0x%"PRIx32"\n", *SCS_CPUID);

    /**
     * 2. The Interrupt Control and State Register, ICSR (B3.2.4)
     * It is always implemented and has no usage constraints.
     * It is R/W with a reset value of 0; there is nothing to configure.
     **/
    VERBOSE_CPU_PRINT("ICSR: 0x%"PRIx32"\n", *SCS_ICSR);

    /**
     * 3. Vector Table Offset Register, VTOR (B3.2.5)
     * It is always implemented and the high 2 bits ([31:30]) may be RAZ/WI.
     * It is R/W and has an ɪᴍᴘʟᴇᴍᴇɴᴛᴀᴛɪᴏɴ ᴅᴇꜰɪɴᴇᴅ initial value.
     * We reset it to the address of our vector table. It should have been
     * that by our boot configuration to our vector table, but let's been
     * careful just in case (or in case someone uses a different bootloader).
     **/

    VERBOSE_CPU_PRINT("VTOR (initial): 0x%"PRIx32"\n", *SCS_VTOR);
    /** "Software can write all 1s to the TBLOFF field and then read the register
     *   to find the maximum supported offset value." */
    *SCB_VTOR = UINT32_MAX;
    uint32_t vtor_supported = *SCB_VTOR;
    VERBOSE_CPU_PRINT("VTOR (max supported): 0x%"PRIx32"\n", vtor_supported);

#define VTOR_RESERVED MASK(6)
#define VTOR_TBLOFF (MASK(31) & VTOR_RESERVED)
    uint32_t tbloff = (word_t)&arm_vector_table[0];
    if (tbloff > vtor_supported) {
        printf("ERROR: vector table location 0x%"PRIx32" is above "
               "maximum supported VTOR 0x%"PRIx32"\n", tbloff, vtor_supported);
        return false;
    }

    VERBOSE_CPU_PRINT("VTOR (desired): 0x%"PRIx32"\n", tbloff);

    assert((tbloff & VTOR_RESERVED) == 0);
    *SCB_VTOR = tbloff;

    VERBOSE_CPU_PRINT("VTOR (final): 0x%"PRIx32"\n", *SCS_VTOR);

    /**
     * 4. Application Interrupt and Reset Control Register, AIRCR
     * It is always implemented and there are no usage constraints.
     * It is RW. Most of these bits control reset, but we do want to configure
     * the priority group binary point. Writing zero to most fields does nothing.
     **/
    VERBOSE_CPU_PRINT("AIRCR: 0x%"PRIx32"\n", *SCS_AIRCR);
    /* VECTKEY, writes must include this */
#define AIRCR_VECTKEY (0xFA05 << 16)
    /* PRIGROUP, bits[10:8] */
#define AIRCR_PRIGROUP(v) ((v) << 8)
    /**
     * We choose to reset the PRIgroup to 0b000, the reset value.
     * See B1.5.4 "Exception priorities and preemption", specifically "Priority
     * grouping" for how this field works. The group priority (for us, [7:1])
     * is the priority for preemption, and we have a single [0] for subpriority,
     * though hardware may support less than 8 priority level bits.
     *
     * TODO: Do we want/need a different value?
     **/
    *SCB_AIRCR = AIRCR_VECTKEY | AIRCR_PRIGROUP(0);

    /**
     * 5. System Control Register, SCR (B3.2.7)
     * It is always implemented and there are no usage constraints.
     * It is RW. Reset value is 0x0. This is about various sleep states.
     **/
    VERBOSE_CPU_PRINT("SCR: 0x%"PRIx32"\n", *SCS_SCR);
    *SCB_SCR = 0x0;

    /**
     * 6. Configuration and Control Register, CCR (B3.2.8)
     * It is always implemented and there are no usage constraints.
     * It is RW with ɪᴍᴘʟᴇᴍᴇɴᴛᴀᴛɪᴏɴ ᴅᴇꜰɪɴᴇᴅ reset.
     **/
    VERBOSE_CPU_PRINT("CCR: 0x%"PRIx32"\n", *SCS_CCR);
    /**
     * Turn on branch prediction, if we can. Turn on the caches, if we can.
     * Turn on STKALIGN (see notes surrounding KERNEL_STACK_ALIGNMENT).
     * Turn on trap on divide by 0.
     * Turn off unaligned trapping.
     * Turn off unprivileged access to software triggered interrupts.
     * Don't let us enter Thread mode with exceptions active. (TODO: do we want this?)
     **/
#define CCR_BP BIT(18)
#define CCR_IC BIT(17)
#define CCR_DC BIT(16)
#define CCR_STKALIGN BIT(9)
#define CCR_BFHFNMIGN BIT(8)
#define CCR_DIV_0_TRP BIT(4)
    *SCB_CCR = CCR_BP | CCR_IC | CCR_DC | CCR_STKALIGN | CCR_BFHFNMIGN | CCR_DIV_0_TRP;

    VERBOSE_CPU_PRINT("CCR (now): 0x%"PRIx32"\n", *SCS_CCR);

    /**
     * 7. System Handler Priority Register 1-3 (B3.2.10 - B3.2.12)
     * It is always implemented and there are no usage constraints.
     * They can be accessed bytewise.
     *
     * The first 3 exceptions have fixed priorities of -3, -2, -1 (Reset, NMI, HardFault).
     * See B1.5.4 for details of exception priorities. Lower numbers are higher
     * priorities, and the reset value is 0.
     *
     * TODO: Decide these values.
     *
     * Since we follow "Use of SVCall and PendSV to avoid critical code regions"
     * from B1-530, we set PendSV and SVCall to have the same, lowest priority.
     *
     * Per Table B1-6, this is 255. If an implementation supports less priority
     * bits, the low order bits are RAZ.
     **/
    static const uint8_t SV_PRIORITY = 0b11111111;

    *SCB_SHPR1 = /* PRI_4 (MemManage)     */ (0 <<  0)
               | /* PRI_5 (BusFault)      */ (0 <<  8)
               | /* PRI_6 (UsageFault)    */ (0 << 16)
               | /* PRI_7 (reserved)      */ (0 << 24);
    *SCB_SHPR2 = /* PRI_8 (reserved)      */ (0 <<  0)
               | /* PRI_9 (reserved)      */ (0 <<  8)
               | /* PRI_10 (reserved)     */ (0 << 16)
               | /* PRI_11 (SVCall)       */ (SV_PRIORITY << 24);
    *SCB_SHPR3 = /* PRI_12 (DebugMonitor) */ (0 <<  0)
               | /* PRI_13 (reserved)     */ (0 <<  8)
               | /* PRI_14 (PendSV)       */ (SV_PRIORITY << 16)
               | /* PRI_15 (SysTick)      */ (0 << 24);

    /**
     * 8. System Handler Control and State Register, SHCSR (B3.2.13)
     * Always implemented, and usage constraints pertain to active states.
     * Reset value is 0x0,
     * We want to enable the UsageFault, BusFault, and MemManageFaults.
     * Writing to any of the pending/active is fine as they should all be 0.
     **/
    VERBOSE_CPU_PRINT("SHCSR: 0x%"PRIx32"\n", *SCS_SHCSR);
#define SHCSR_USGFAULTENA BIT(18)
#define SHCSR_BUSFAULTENA BIT(17)
#define SHCSR_MEMFAULTENA BIT(16)
    *SCB_SHCSR = SHCSR_USGFAULTENA | SHCSR_BUSFAULTENA | SHCSR_MEMFAULTENA;

    /* Ensure all our writes are in effect */
    dsb();
    isb();

    /* Now we are done configuring the SCS
       If adding FPU support, add the configuration of those registers here: */

    return true;
}

BOOT_CODE static void init_irqs(cap_t root_cnode_cap)
{
#if 0
    unsigned i;

    for (i = 0; i <= maxIRQ ; i++) {
        setIRQState(IRQInactive, CORE_IRQ_TO_IRQT(0, i));
    }

    setIRQState(IRQTimer, CORE_IRQ_TO_IRQT(0, KERNEL_TIMER_IRQ));
#endif

    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(root_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_CODE static bool_t try_init_kernel(void)
{
#ifdef CONFIG_PRINTING
    plat_uart_init();
#endif

    printf("Bootstrapping kernel\n");

    /* initialise the CPU */
    if (!init_cpu()) {
        printf("ERROR: CPU init failed\n");
        return false;
    }

    if (!reserve_region(system_control_space_reg)) {
        printf("ERROR: failed to reserve System Control Space region\n");
        return false;
    }

    if (!plat_init_freemem()) {
        printf("ERROR: free memory management initialisation failed\n");
        return false;
    }

    /* create the root cnode */
    cap_t root_cnode_cap = create_root_cnode();
    if (cap_get_capType(root_cnode_cap) == cap_null_cap) {
        printf("ERROR: root c-node creation failed\n");
        return false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(root_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(root_cnode_cap);

    populate_bi_frame(0, CONFIG_MAX_NUM_NODES, /* ipcbuf_vptr */ 0x0, 0x0);

    /* create the idle thread */
    create_idle_thread();

    tcb_t *initial = create_initial_thread(root_cnode_cap,
                                           /* ui_entry */ 0x80000000,
                                           /* ui_initial_stack */ 0x80000100);

    if (initial == NULL) {
        printf("ERROR: could not create initial thread\n");
        return false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    if (!create_untypeds(root_cnode_cap)) {
        printf("ERROR: could not create untypeds for kernel image boot memory\n");
        return false;
    }

    printf("Booting all finished, dropping to user space\n");

    return true;
}


BOOT_CODE VISIBLE void init_kernel(void)
{
    bool_t result;

    result = try_init_kernel();

    if (!result) {
        fail("ERROR: kernel init failed");
    }

    schedule();
    activateThread();
}
