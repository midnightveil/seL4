/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <kernel/vspace.h>
#include <object/structures.h>
#include <arch/machine.h>
#include <arch/model/statedata.h>
#include <arch/object/objecttype.h>

bool_t Arch_isFrameType(word_t type)
{
    switch (type) {
    default:
        return false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    // deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    default:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    return cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    return cap;
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {
    default:
        break;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    return fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    default:
        return false;
    }
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    return Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    switch (t) {
    default:
        fail("Invalid object type");
        return 0;
    }
}

cap_t Arch_createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
    switch (t) {
    default:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *buffer)
{
    return EXCEPTION_SYSCALL_ERROR;
}

void Arch_prepareThreadDelete(tcb_t * thread) {

}

