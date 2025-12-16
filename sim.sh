#!/usr/bin/env sh

set -euo pipefail

# M4F
# qemu-system-arm -M mps2-an386 -nographic -kernel "$@"
# M33
qemu-system-arm -M mps3-an524,remap=BRAM -nographic -kernel "$@"

# use --list-params
#~/ARM/avh-linux-x86/bin/FVP_MPS2_Cortex-M4 \
#    -C armcortexm4ct.semihosting-enable=0 \
#    -C fvp_mps2.UART0.out_file='-' \
#    -q \
#    -a "$@"
