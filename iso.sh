#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/qios.kernel isodir/boot/qios.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "QiOS" {
	multiboot2 /boot/qios.kernel
}
EOF
grub-mkrescue -o qios.iso isodir
