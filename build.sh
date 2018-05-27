#!/bin/sh

KERNEL_DIR=$PWD
ZIMAGE=$KERNEL_DIR/out/arch/arm/boot/zImage
BUILD_START=$(date +"%s")
blue='\033[0;34m'
cyan='\033[0;36m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'

export ARCH=arm
export CROSS_COMPILE=$HOME/arm-eabi-4.8/bin/arm-eabi-
export KBUILD_BUILD_USER="Yuvraj"
export KBUILD_BUILD_HOST="∆Thestral"

compile_kernel()
{
echo -e "$blue************************************************"
echo "                     Compiling  kernel                  "
echo -e "***********************************************$nocol"
mkdir -p out
make Q340_defconfig O=out/
make -j4 O=out/
if [ ! -e $ZIMAGE ]
then
echo -e "$red Kernel Compilation failed! Fix the errors! $nocol"
exit 1
fi
}

case $1 in
clean)
make ARCH=arm -j$(nproc --all) clean mrproper
rm -rf include/linux/autoconf.h
;;
*)
compile_kernel
;;
esac
BUILD_END=$(date +"%s")
DIFF=$(($BUILD_END - $BUILD_START))
echo -e "$yellow Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol"
