#!/bin/sh
set -e
. ./qemu.sh

gdb -x .gdbinit

