#!/bin/bash
# Script for running debug mode
DEBUG=0
ISO=0
QEMU=qemu-system-i386
for args in $@
do
	if [ "$args" == "debug" ]; then
		DEBUG=1
	fi
	if [ "$args" == "verbose" ]; then
		set -x
	fi
	if [ "$args" == "iso" ]; then
		ISO=1
	fi
	if [ "$args" == "help" ]; then
		echo -e "run.sh\nUsage: ./run.sh [debug] [verbose] [iso] [help]\n"
		echo -e "\tdebug\tCompile source code with debugging flag."
		echo -e "\tverbose\tEcho commands execute in this script."
		echo -e "\trebuild\tBuild everything from including boot disk image."
		echo -e "\tiso\tUse iso image instead of disk image."
		echo -e "\thelp\tPrint this help text.\n"
		exit 0
	fi
done

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
WORKSPACE="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

IMAGE="drive format=raw,file=$WORKSPACE/bin/disk.img"
if [ "$ISO" == 1 ]; then
	IMAGE="cdrom $WORKSPACE/bin/boot.iso"
fi

if [ "$DEBUG" == 1 ]; then
	echo "Run debug mode..."
	#qemu-system-i386 -drive format=raw,file="$WORKSPACE/../bin/disk.img" -m 2048M -daemonize\
	${QEMU} -${IMAGE} -machine q35 -m 2048M -daemonize -cpu qemu32,pae,acpi,pat \
	-D qemu.log -d guest_errors -serial file:boot.log -s -S
	gdb "$WORKSPACE/bin/kernel.elf" -ex "target remote :1234" --tui # --symbols="$WORKSPACE/../bin/shos.sym"
else
	echo "Run normal..."
	${QEMU} -${IMAGE} -machine q35 -m 2048M -daemonize -cpu qemu32,pae,acpi,pat \
	-D qemu.log -d guest_errors -serial file:./boot.log #-cpu Nehalem
fi
