#!/usr/bin/env sh
#
# Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
# Copyright 2022, Capgemini Engineering
#
# SPDX-License-Identifier: GPL-2.0-only
#

set -eo pipefail

if [ -z "$1" ]; then
    echo "Usage: $0 </path/to/linux/kernel>"
    exit 1
fi

destdir=`pwd`

cd "$1" || exit 1
echo Using DTS from Linux $(make -s kernelversion)
make ARCH=arm multi_v7_defconfig
make ARCH=arm -j4 dtbs

make ARCH=arm64 defconfig
make ARCH=arm64 -j4 dtbs

make ARCH=riscv defconfig
make ARCH=riscv -j4 dtbs

LICENSE="/*
 * Copyright Linux Kernel Team
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * This file is derived from an intermediate build stage of the
 * Linux kernel. The licenses of all input files to this process
 * are compatible with GPL-2.0-only.
 */
"

ARM_DTBS="
ti/omap/am335x-bone=am335x-bone
ti/omap/am335x-boneblack=am335x-boneblack
ti/omap/am335x-boneblue=am335x-boneblue
broadcom/bcm2837-rpi-3-b=rpi3
samsung/exynos4412-odroidx=exynos4
samsung/exynos5250-arndale=exynos5250
samsung/exynos5410-odroidxu=exynos5410
samsung/exynos5422-odroidxu4=exynos5422
nxp/imx/imx6q-sabrelite=sabre
nxp/imx/imx6q-wandboard-revd1=wandq
nxp/imx/imx7d-sdb=imx7sabre
ti/omap/omap3-beagle=omap3
qcom/qcom-apq8064-ifc6410=apq8064
allwinner/sun7i-a20-cubietruck=allwinnerA20
nvidia/tegra124-jetson-tk1=tk1
xilinx/zynq-zc706=zynq7000
"

ARM64_DTBS="
amlogic/meson-sm1-odroid-c4=odroidc4
amlogic/meson-gxbb-odroidc2=odroidc2
hisilicon/hi6220-hikey=hikey
nvidia/tegra210-p2371-2180=tx1
xilinx/avnet-ultra96-rev1=ultra96
xilinx/zynqmp-zcu102-rev1.0=zynqmp
freescale/imx8mq-evk=imx8mq-evk
freescale/imx8mm-evk=imx8mm-evk
rockchip/rk3399-rockpro64=rockpro64
rockchip/rk3566-quartz64-a=quartz64
broadcom/bcm2711-rpi-4-b=rpi4
"

RISCV_DTBS="
starfive/jh7110-pine64-star64=star64
"

extract_dts() {
    dtb=`echo $1| sed 's/=.*$//'`
    platform=`echo $1| sed 's/^.*=//'`
    echo "'$dtb'" = "'$platform'"
    dtc -I dtb -O dts -o $2/$platform.dts.tmp $3/$dtb.dtb
    echo "$LICENSE" > $2/$platform.dts
    cat $2/$platform.dts.tmp >> $2/$platform.dts
    rm $2/$platform.dts.tmp
}

for entry in $ARM_DTBS; do
    extract_dts $entry $destdir arch/arm/boot/dts
done

for entry in $ARM64_DTBS; do
    extract_dts $entry $destdir arch/arm64/boot/dts
done

for entry in $RISCV_DTBS; do
    extract_dts $entry $destdir arch/riscv/boot/dts
done
