#include <arch/bootinfo.h>
#include <arch/kernel/boot.h>
#include <basic_types.h>
#include <kernel/boot.h>
#include <linker.h>
#include <machine.h>
#include <plat/machine.h>

extern char _sram_start[1], _sram_end[1];

static const p_region_t BOOT_RODATA normal_memory[] = {
    { .start = (paddr_t)&_sram_start,  .end = (paddr_t)&_sram_end  },
};

BOOT_BSS static region_t res_reg[NUM_RESERVED_REGIONS];

BOOT_CODE bool_t plat_init_freemem(void)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());
    int index = 1;

    return init_freemem(ARRAY_SIZE(normal_memory), normal_memory,
                        index, res_reg,
                        /* the size of the it_v_reg doesn't matter for MPUs */
                        (v_region_t){0}, 0);
}
