#pragma once

#include <basic_types.h>
#include <linker.h>
#include <util.h>

BOOT_CODE VISIBLE void init_kernel(void);
#ifdef CONFIG_PRINTING
BOOT_CODE void plat_uart_init(void);
#endif

BOOT_CODE bool_t plat_init_freemem(void);
