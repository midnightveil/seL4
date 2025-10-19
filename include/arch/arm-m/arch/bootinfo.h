#pragma once

#include <config.h>
#include <plat/machine/devices_gen.h>

#define MAX_NUM_FREEMEM_REG 16
#define NUM_RESERVED_REGIONS 3

#define MAX_NUM_RESV_REG (MAX_NUM_FREEMEM_REG + NUM_KERNEL_DEVICE_FRAMES + \
                          NUM_RESERVED_REGIONS)
