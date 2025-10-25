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
    /* Check for the presence of the optional floating point extensions FPv4-SP
       and FPv5. Reference A2.5 and B3.2.20 of ARMv7-M ARM DDI 0403E.e.
    */
    bool_t haveHWFPU = fp_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    if (haveHWFPU) {
        // TODO: Support CONFIG_HAVE_FPU.
        printf("Platform has an FPU, disabling\n");
        fp_HWDisable();
    }

    return true;
}

void init_kernel(void)
{
    printf("Bootstrapping kernel\n");

    /* initialise the CPU */
    if (!init_cpu()) {
        printf("ERROR: CPU init failed\n");
        halt();
    }

    while (1);
}
