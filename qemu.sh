#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST)  -S -gdb tcp::1234 -boot d -cdrom qios.iso -m 1024M