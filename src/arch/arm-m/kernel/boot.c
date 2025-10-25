#include <util.h>
#include <machine/io.h>

void init_kernel(void) NORETURN;

void init_kernel(void)
{
    printf("Hello\n");

    while (1);
}
