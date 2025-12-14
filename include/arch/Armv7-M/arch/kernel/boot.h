#pragma once

#include <linker.h>
#include <util.h>

BOOT_CODE VISIBLE void init_kernel(void);
#ifdef CONFIG_PRINTING
BOOT_CODE void plat_uart_init(void);
#endif
