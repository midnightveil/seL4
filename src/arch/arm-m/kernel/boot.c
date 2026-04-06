#include <arch/kernel/boot.h>
#include <arch/kernel/traps.h>
#include <arch/machine/registerset.h>
#include <arch/machine/timer.h>
#include <kernel/boot.h>
#include <kernel/thread.h>
#include <linker.h>
#include <machine.h>
#include <machine/io.h>
#include <machine/fpu.h>
#include <object/objecttype.h>
#include <util.h>

#include <sel4/userspace_setup.h>

extern char ki_userspace_start[1];

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

    /* Out of reset we would expect PRIMASK, BASEPRI and FAULTMASK to be 0.
       However, we do want to disable interrupts (but not faults) within
       the kernel, so we set PRIMASK.PM (equivalent to cpsid i)
     */
#define PRIMASK_PM BIT(0)
    MSR("PRIMASK", PRIMASK_PM);
    MSR("FAULTMASK", 0);
    MSR("BASEPRI", 0);
    /**
     * We force a 'Context Synchronisation Event' for the MSR write(s) now
     * so that we can mess around with interrupts, and their priorities without
     * being preempted.
     **/
    isb();

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
    VERBOSE_CPU_PRINT("CPUID: 0x%"PRIx32"\n", *SCB_CPUID);

    /**
     * 2. The Interrupt Control and State Register, ICSR (B3.2.4)
     * It is always implemented and has no usage constraints.
     * It is R/W with a reset value of 0; there is nothing to configure.
     **/
    VERBOSE_CPU_PRINT("ICSR: 0x%"PRIx32"\n", *SCB_ICSR);

    /**
     * 3. Vector Table Offset Register, VTOR (B3.2.5)
     * It is always implemented and the high 2 bits ([31:30]) may be RAZ/WI.
     * It is R/W and has an ɪᴍᴘʟᴇᴍᴇɴᴛᴀᴛɪᴏɴ ᴅᴇꜰɪɴᴇᴅ initial value.
     * We reset it to the address of our vector table. It should have been
     * that by our boot configuration to our vector table, but let's been
     * careful just in case (or in case someone uses a different bootloader).
     **/

    VERBOSE_CPU_PRINT("VTOR (initial): 0x%"PRIx32"\n", *SCB_VTOR);
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

    VERBOSE_CPU_PRINT("VTOR (final): 0x%"PRIx32"\n", *SCB_VTOR);

    /**
     * 4. Application Interrupt and Reset Control Register, AIRCR
     * It is always implemented and there are no usage constraints.
     * It is RW. Most of these bits control reset, but we do want to configure
     * the priority group binary point. Writing zero to most fields does nothing.
     **/
    VERBOSE_CPU_PRINT("AIRCR: 0x%"PRIx32"\n", *SCB_AIRCR);
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
    VERBOSE_CPU_PRINT("SCR: 0x%"PRIx32"\n", *SCB_SCR);
    *SCB_SCR = 0x0;

    /**
     * 6. Configuration and Control Register, CCR (B3.2.8)
     * It is always implemented and there are no usage constraints.
     * It is RW with ɪᴍᴘʟᴇᴍᴇɴᴛᴀᴛɪᴏɴ ᴅᴇꜰɪɴᴇᴅ reset.
     **/
    VERBOSE_CPU_PRINT("CCR: 0x%"PRIx32"\n", *SCB_CCR);
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

    VERBOSE_CPU_PRINT("CCR (now): 0x%"PRIx32"\n", *SCB_CCR);

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
               | /* PRI_15 (SysTick)      */ (SV_PRIORITY << 24);

    /**
     * 8. System Handler Control and State Register, SHCSR (B3.2.13)
     * Always implemented, and usage constraints pertain to active states.
     * Reset value is 0x0,
     * We want to enable the UsageFault, BusFault, and MemManageFaults.
     * Writing to any of the pending/active is fine as they should all be 0.
     **/
    VERBOSE_CPU_PRINT("SHCSR: 0x%"PRIx32"\n", *SCB_SHCSR);

    *SCB_SHCSR = SHCSR_USGFAULTENA | SHCSR_BUSFAULTENA | SHCSR_MEMFAULTENA | SHCSR_SECUREFAULTENA;

    /**
     * Make sure that writes to the System Control Space registers (in *memory*)
     *  have completed and are architecturally visible. (B7.2.16 Armv8)
     **/
    dsb();
    isb();

    /* Now we are done configuring the SCS
       If adding FPU support, add the configuration of those registers here: */

// XXX: elsewhere
#ifdef CONFIG_PLAT_RP2350
    /**
     * The RP2350 has 'Access control' registers in addition to the architectural
     * IDAU and SAU-defined regions.
     **/

    /* Per 10.6 of datasheet, all access control writes need this */
    #define ACCESSCTRL_PASSWORD 0xacce0000

    #define ACCESSCTRL_BASE 0x40060000
    #define ACCESSCTRL_REG32(offset) ((volatile uint32_t *)(ACCESSCTRL_BASE + (offset)))

    /* Table 911 / 10.6.3 list of registers */
    #define ACCESSCTRL_CFGRESET 0x08
    #define ACCESSCTRL_CLOCKS   0xC0
    #define ACCESSCTRL_XOSC     0xC4

    // reset to defaults for most registers
    // *ACCESSCTRL_REG32(ACCESSCTRL_CFGRESET) = ACCESSCTRL_PASSWORD | BIT(0);

    // TODO: we might want to configure more registers -- or make this a syscall
    //       but for now...
    // set the SU (secure, unprivilged) bits on
    *ACCESSCTRL_REG32(ACCESSCTRL_CLOCKS) |= ACCESSCTRL_PASSWORD | BIT(2);
    *ACCESSCTRL_REG32(ACCESSCTRL_XOSC) |= ACCESSCTRL_PASSWORD | BIT(2);
#endif

    /**
     * Now we want to switch ourselves into 'Handler' mode, so that we can
     * perform the standard ExceptionReturn into the first task. (We can't
     * emulate all the features that Handler mode does, such as changing
     * CONTROL_nPRIV or enabling SysTick without it preempting (or specifically
     * then turning off PRIMASK without a race condition).
     * Unfortunately, there's no way to do this in Cortex-M explicitly from
     * Thread mode, which is what we start in. One way to do this would be
     * to make the kernel perform an 'SVC' call into itself. This would get us
     * into Handler mode, but creates its own issue: specifically, we need
     * to switch out the SVC handler during boot to not have a cost in every
     * syscall; but doing so requires a duplicate set of vector tables. Which,
     * would be fine, as they could be thrown away after boot, but is annoying.
     *
     *
     **/

    // FIXME: https://developer.arm.com/documentation/100235/0003/the-cortex-m33-peripherals/system-control-block/system-handler-control-and-state-register?lang=en
    //       what are the actual requirements? is it just in relation to fault entry/exit
    //       and having the right stuff to stack (which we do)
    //
    // > When updating the SHCSR, Arm recommends using a read-modify-write sequence, to avoid unintended effects on the state of the exception handlers.
    // ?? atomics? or does it just mean |= ??


    // pseudocode: IsActiveForState; SetActive; RawExecutionPriority\
    // ValidateExceptionReturn
    //  gets excNumber
    //      checks isActiveForState
    //      calls DeActivate
    //      calls CreateException
    // Also: ExceptionReturn()
    //      integer returningExceptionNumber = UInt(IPSR.Exception);
    //      then validates that number


    // B3.22 Exception Return
    // $R_NCQN$   On exception return the following procedures are carried out:
    //            - secure mode stuff
    //            - check that the exception number being returned from, as held in the IPSR, is shown as active
    //          A check that if the return is to Thread mode, the value that is restored to the IPSR from the RETPSR is zero,
    //          or that if the return is to Handler mode, the value that is restored to the IPSR from the RETPSR is nonzero. If
    //          this check fails:

    // what is RETPSR

    // printf("hi\n");

    // *SCB_SHCSR |= SHCSR_PENDSVACT | SHCSR_PENDSVSET;
    // isb();

    // printf("IPSR: 0x%lx\n", MRS("IPSR"));
    // printf("SHCSR: 0x%x\n", *SCB_SHCSR);

    // That doesn't work. OK.

    initTimer();

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
        printf("ERROR: root cnode creation failed\n");
        return false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(root_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(root_cnode_cap);

    // XX: ipcbuf_vptr is a physical address heren
    populate_bi_frame(0, CONFIG_MAX_NUM_NODES, /* ipcbuf_vptr */ rootserver.ipc_buf, 0x0);

    /* create the idle thread */
    create_idle_thread();

    seL4_UserspaceSetupData_t *setup_data = (void *)&ki_userspace_start;

    if (!((setup_data->magic[0] == LIBSEL4_USERSPACE_SETUP_MAGIC[0]) &&
          (setup_data->magic[1] == LIBSEL4_USERSPACE_SETUP_MAGIC[1]) &&
          (setup_data->magic[2] == LIBSEL4_USERSPACE_SETUP_MAGIC[2]) &&
          (setup_data->magic[3] == LIBSEL4_USERSPACE_SETUP_MAGIC[3]))) {
        printf("ERROR: userspace setup data magic does not match\n");
        return false;
    }

    printf("Userspace setup data:\n");
    printf("    entrypoint: 0x%"SEL4_PRIx_word"\n", setup_data->entrypoint);
    printf("    initial sp: 0x%"SEL4_PRIx_word"\n", setup_data->initial_sp);

    tcb_t *initial = create_initial_thread(root_cnode_cap,
                                           /* ui_entry */ (word_t)(setup_data->entrypoint) | 1, // or 1 for thumb mode
                                           /* ui_initial_stack */ (word_t)(setup_data->initial_sp),
                                           /* bootinfo_frame */ rootserver.boot_info);

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

    /* finalise the bootinfo frame */
    bi_finalise();

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
