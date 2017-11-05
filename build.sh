#!/bin/bash
# Build script for Sin Hing OS (Barebone)

DEBUG=0
if [ ! -z "$1" ]; then
	if [ "$1" == "debug" ]; then
		set -x
		DEBUG=1
	fi
fi

# Variables

NASM=" -felf32 "
CFLAGS=" -std=gnu99 -ffreestanding -O2 -Wall -Wextra "
if [ "$DEBUG" == 1 ]; then
	NASM=$NASM"-gdwarf "
	CFLAGS=$CFLAGS"-g "
fi

# WORKSPACE is the directory where this script located.
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
WORKSPACE="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

SOURCEDIR="$WORKSPACE"/src
INCLUDEDIR="$SOURCEDIR"/inc
OBJDIR="$WORKSPACE"/obj
OUTPUTDIR="$WORKSPACE"/bin
LINKERSCRIPT="$SOURCEDIR"/linker.located
OUTPUTTARGET="$OUTPUTDIR"/boot/shos.bin
ISODIR="$OUTPUTDIR"/isodir
ISOBOOT="$ISODIR"/boot
ISOGRUB="$ISOBOOT"/grub
OBJLIST=" "

# Compiling all source file
for f in $(find $SOURCEDIR -name '*.c' -or -name '*.asm'); do
	filename=$(basename "$f")
	ext="${filename##*.}"
	target="$OBJDIR"/${filename%.*}.o
	if [ "$ext" == "asm" ]; then
		nasm $NASM -o "$target" "$f"
	else
		i686-elf-gcc -c "$f" -I "$INCLUDEDIR" -o "$target" $CFLAGS
	fi
	if [ $? -ne 0 ]; then
		exit
	fi
	OBJLIST+="$target "
done

i686-elf-gcc -T "$LINKERSCRIPT" -o "$OUTPUTTARGET" -ffreestanding -O2 -nostdlib -I"$SOURCEDIR"/"$1"/ $OBJLIST -lgcc
if [ $? -ne 0 ]; then
	exit
fi

# Verification and ISO creation
if grub-file --is-x86-multiboot2 "$OUTPUTTARGET"; then
  echo multiboot confirmed
else
  echo the file is not multiboot2
  exit -2
fi

if [ "$DEBUG" == 1 ]; then
	objcopy --only-keep-debug "$OUTPUTTARGET" "$OUTPUTDIR"/shos.sym
	chmod -x "$OUTPUTDIR"/shos.sym
fi

mkdir -p "$ISOGRUB"
cp "$OUTPUTTARGET" "$ISOBOOT"/shos.bin
cp "$SOURCEDIR"/grub.cfg "$ISOGRUB"/grub.cfg

grub-mkrescue -o "$OUTPUTDIR"/simpleos.iso "$ISODIR"
if [ $? -ne 0 ]; then
	exit
fi
echo Done.