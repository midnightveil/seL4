#include <linker.h>
#include <machine/io.h>
#include <machine/fpu.h>
#include <util.h>

void init_kernel(void) NORETURN;

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

void init_kernel(void)
{
    void uart_init(void);
    uart_init();

    printf("Bootstrapping kernel\n");

    /* initialise the CPU */
    if (!init_cpu()) {
        printf("ERROR: CPU init failed\n");
        halt();
    }

    halt();
    while (1);
}
