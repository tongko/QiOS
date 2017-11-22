#!/bin/bash
# Script for running debug mode

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
WORKSPACE="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

qemu-system-i386 -hda "$WORKSPACE/../bin/disk.img" -cpu Nehalem -m 2048M -daemonize -s -S

gdb "$WORKSPACE/../bin/boot/shos.bin" -ex "target remote :1234" --symbols="$WORKSPACE/../bin/shos.sym" --tui
