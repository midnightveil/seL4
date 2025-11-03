#pragma once

/**
 * Armv7-M implementations treat SP bits[1:0] as RAZ/WI. Arm strongly recommends that software treats SP
 * bits[1:0] as SBZP for maximum portability across Armv7 profiles.
 *
 * - ARMv7-M ARM DDI 0403E.e, B1.4.1
 **/
#define KERNEL_STACK_ALIGNMENT 4
