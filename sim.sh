#!/usr/bin/env sh

set -euo pipefail

qemu-system-arm -M ast1030-evb -nographic -kernel "$@"
