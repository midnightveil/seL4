#include <util.h>
#include <linker.h>
#include <machine/io.h>

void init_kernel(void) NORETURN;

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_CODE static bool_t init_cpu(void)
{
    return false;
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
