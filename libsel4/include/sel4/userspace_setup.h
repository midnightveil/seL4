#pragma once

/* shared between kernel and libsel4 */

#define LIBSEL4_USERSPACE_SETUP_MAGIC ((char []){'m', 'e', 'L', '4'})

typedef struct seL4_UserspaceSetupData_ {
    char magic[4];
    seL4_Word entrypoint;
    seL4_Word initial_sp;
} seL4_UserspaceSetupData_t;
