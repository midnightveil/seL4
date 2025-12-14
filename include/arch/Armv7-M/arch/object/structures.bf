#include <config.h>

-- Default base size: uint32_t
base 32

-- Including the common structures_32.bf is necessary because
-- we need the structures to be visible here when building
-- the capType
#include <object/structures_32.bf>

-- NB: odd numbers are arch caps (see isArchCap())
tagged_union cap capType {
    mask 4 0xe
    mask 8 0x0e

    -- 4-bit tag caps
    tag null_cap             0
    tag untyped_cap          2
    tag endpoint_cap         4
    tag notification_cap     6
    tag reply_cap            8
    tag cnode_cap           10
    tag thread_cap          12
    -- Do not extend even 4-bit caps types beyond 12, as we use
    -- 14 (0xe) to determine which caps are 8-bit.

    -- 4-bit tag arch caps
    -- tag small_frame_cap      1
    -- tag frame_cap            3
    -- tag asid_control_cap    11
    -- Do not extend odd 4-bit caps types beyond 13, as we use
    -- 15 (0xf) to determine which caps are 8-bit.

    -- 8-bit tag caps
    tag irq_control_cap     0x0e
    tag irq_handler_cap     0x1e
    tag zombie_cap          0x2e
    tag domain_cap          0x3e
#ifdef CONFIG_KERNEL_MCS
    tag sched_context_cap   0x4e
    tag sched_control_cap   0x5e
#endif
}

#include <sel4/arch/shared_types.bf>
