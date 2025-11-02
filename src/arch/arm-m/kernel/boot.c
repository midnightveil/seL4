#include <arch/kernel/boot.h>
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
    /* At this point we're roughly assuming that the CPU is in reset.
       ARMv7-M ARM DDI 0403E.e B1.5.5 "Reset behaviour" shows a set of registers
       that are set at system reset, and where possible we want to initialise
       or reset these values as desired.
     */

    /* We first assume that we are in "Thread Mode" and "Privileged execution".
       Reference ARMv7-M ARM DDI 0403E.e B1.3.1 and B1.4.4.
       We can't check the thread mode, but we can check the privilege level
       and available stacks.
     */

#define CONTROL_nPRIV BIT(0)
#define CONTROL_SPSEL BIT(1)

    word_t CONTROL = MRS("CONTROL");
    if (CONTROL & CONTROL_nPRIV) {
        printf("Execution is unpriviliged\n");
        return false;
    } else if (CONTROL & CONTROL_SPSEL) {
        /* If using our bootcode, the initial stack the vector table starts with
           is the SP_main per B1.5.5 */
        printf("Stack pointer is SP_process, expected SP_main\n");
        return false;
    }

    /* Read the Program Status Register xPSR, ref ARMv7-M ARM DDI 0403E.e B1.4.2
       (a combination of APSR, IPSR, and EPSR).
       Note that the EPSR is always read-as-zero when read via an MRS.
     */
#define xPSR_IPSR MASK(8)
#define xPSR_EPSR_T BIT(24)
    word_t xPSR = MRS("xPSR");
    printf("xPSR: %"SEL4_PRIx_word"\n", xPSR);
    if ((xPSR & xPSR_IPSR) != 0) {
        /* The IPSR contains the value 0 in Thread Mode, or the exception number
           in Handler mode. */
        printf("In Handler mode with Exception Number %"SEL4_PRIx_word"\n", xPSR & xPSR_IPSR);
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

BOOT_BSS static region_t res_reg[NUM_RESERVED_REGIONS];

BOOT_CODE static bool_t arch_init_freemem(void)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());
    int index = 1;

    // /* reserve the user image region */
    // if (index >= ARRAY_SIZE(res_reg)) {
    //     printf("ERROR: no slot to add user image to reserved regions\n");
    //     return false;
    // }
    // res_reg[index] = ui_reg;
    // index += 1;

    /* avail_p_regs comes from the auto-generated code */
    return init_freemem(ARRAY_SIZE(avail_p_regs), avail_p_regs,
                        index, res_reg,
                        (v_region_t){0}, 0);
}

BOOT_CODE static bool_t try_init_kernel(void)
{
    void uart_init(void);
    uart_init();

    printf("Bootstrapping kernel\n");

    /* initialise the CPU */
    if (!init_cpu()) {
        printf("ERROR: CPU init failed\n");
        return false;
    }

    if (!arch_init_freemem()) {
        printf("ERROR: free memory management initialization failed\n");
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

#ifdef CONFIG_KERNEL_MCS
    init_sched_control(root_cnode_cap, CONFIG_MAX_NUM_NODES);

    NODE_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    tcb_t *initial = create_initial_thread(
                         root_cnode_cap,
                         // it_pd_cap,
                         /* ui_entry */ 0x80000000
                         // bi_frame_vptr,
                         // ipcbuf_vptr,
                         // ipcbuf_cap
                     );

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

    printf("Booting all finished, dropped to user space\n");
    return true;
}


BOOT_CODE VISIBLE void init_kernel(void)
{
    bool_t result;

    result = try_init_kernel();

    if (!result) {
        fail("ERROR: kernel init failed");
    }

#ifdef CONFIG_KERNEL_MCS
    NODE_STATE(ksCurTime) = getCurrentTime();
    NODE_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}
