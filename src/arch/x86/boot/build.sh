#!/bin/bash
# Build script for Sin Hing OS (Barebone)

DEBUG=0
REBUILD=0
REBUILDISO=0
SKIP = 0
for args in $@
do
	if [ "$args" == "debug" ]; then
		DEBUG=1
	fi
	if [ "$args" == "verbose" ]; then
		set -x
	fi
	if [ "$args" == "rebuild" ]; then
		REBUILD=1
	fi
	if [ "$args" == "iso" ]; then
		REBUILDISO=1
	fi
	if [ "$args" == "help" ]; then
		echo -e "build.sh\nUsage: ./build.sh [debug] [verbose] [rebuild] [help]\n"
		echo -e "\tdebug\tCompile source code with debugging flag."
		echo -e "\tverbose\tEcho commands execute in this script."
		echo -e "\trebuild\tBuild everything from including boot disk image."
		echo -e "\thelp\tPrint this help text.\n"
		exit 0
	fi
done

# Variables

NASM=" -f elf "
CFLAGS=" -std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel "
if [ "$DEBUG" == 1 ]; then
	NASM=$NASM" -g "
	CFLAGS=$CFLAGS"-g -O0"	# Debug symbol + no optimization
else
	CFLAGS=$CFLAGS"-O2 "
fi

# WORKSPACE is the directory where this script located.
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
WORKSPACE="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

SRCDIR="$WORKSPACE"
SRCBOOT="$SRCDIR"
OBJDIR="$WORKSPACE"
OUTPUTDIR="$WORKSPACE"
OUTPUTTARGET="$OUTPUTDIR/kernel.elf"
GRUBDIR="$OUTPUTDIR/boot/grub"
DISKIMG="$OUTPUTDIR/disk.img"
ISODIR="$OUTPUTDIR"/isodir
ISOBOOT="$ISODIR"/boot
ISOGRUB="$ISOBOOT"/grub
LINKERSCRIPT="$SRCDIR"/link.ld
OBJLIST=" "

# Compiling all source file
for f in $(find $SRCBOOT -name '*.c' -or -name '*.asm'); do
	filename=$(basename "$f")
	ext="${filename##*.}"
	target="$OBJDIR"/${filename%.*}.o
	echo "Compiling $f"
	if [ "$ext" == "asm" ]; then
		nasm $NASM -o "$target" "$f"
	else
		i686-elf-gcc -c "$f" -o "$target" $CFLAGS
	fi
	if [ $? -ne 0 ]; then
		exit
	fi
	OBJLIST+="$target "
done

i686-elf-gcc -T "$LINKERSCRIPT" -o "$OUTPUTTARGET" -ffreestanding -O2 -nostdlib $OBJLIST -lgcc --verbose
if [ $? -ne 0 ]; then
	exit
fi

# Verification and ISO creation
if grub-file --is-x86-multiboot2 "$OUTPUTTARGET"; then
  echo multiboot2 confirmed
else
  echo the file is not multiboot2
  exit -2
fi

# if [ "$DEBUG" == 1 ]; then
# 	objcopy --only-keep-debug "$OUTPUTTARGET" "$OUTPUTDIR"/shos.sym
# 	if [ $? -ne 0 ]; then
# 		exit -3
# 	fi
# 	chmod -x "$OUTPUTDIR"/shos.sym
# 	if [ $? -ne 0 ]; then
# 		exit -4
# 	fi
# fi

# Build iso image.
if [ "$REBUILDISO" == 1 ]; then
	mkdir -p "$ISOGRUB"
	cp "$OUTPUTTARGET" "$ISOBOOT"
	cp "$SRCDIR/grub.cfg" "$ISOGRUB"
	grub-mkrescue -o "$OUTPUTDIR/boot.iso" "$ISODIR"

	#rm -rf "$ISODIR"
	SKIP=1
fi

if [ "$SKIP" == 0 ]; then
# Setup Disk Image
if [ "$REBUILD" == 1 ]; then
	# Create disk image
	dd if=/dev/zero of="$DISKIMG" bs=512 count=131072
	if [ $? -ne 0 ]; then
		exit -5
	fi
	sed -e 's/\s*\([\+0-9a-zA-Z]*\).*/\1/' << EOF | fdisk ${DISKIMG}
	  n # new partition
	  p # primary partition
	    # partition number 1
	    # default - start at beginning of disk 
	    # default - end at tail of disk
	  a # make a partition bootable
	  w # write the partition table
EOF

	# Setup two loop devices. One will be used for writing GRUB and its 
	# additional codes to MBR, and the second will be used for mounting 
	# filesystem of your operating system.
	LD0=$(sudo losetup --find --show "$DISKIMG")
	if [ $? -ne 0 ]; then
		exit -6
	fi
	LD1=$(sudo losetup --find --show "$DISKIMG" -o 1048576)
	if [ $? -ne 0 ]; then
		exit -6
	fi

	sudo mke2fs "$LD1"
	if [ $? -ne 0 ]; then
		exit -7
	fi
	sudo mount "$LD1" /mnt
	if [ $? -ne 0 ]; then
		exit -7
	fi

	sudo grub-install --boot-directory=/mnt/boot --no-floppy --modules="normal part_msdos ext2 multiboot" --target=i386-pc "$LD0"
	if [ $? -ne 0 ]; then
		exit -8
	fi
else
	LD0=$(sudo losetup --find --show "$DISKIMG")
	if [ $? -ne 0 ]; then
		exit -6
	fi
	LD1=$(sudo losetup --find --show "$DISKIMG" -o 1048576)
	if [ $? -ne 0 ]; then
		exit -6
	fi

	sudo mount "$LD1" /mnt
	if [ $? -ne 0 ]; then
		exit -7
	fi
fi

# Copy kernel to disk.img
echo "Copy grub.cfg to disk.img"
sudo cp "$SRCDIR/grub.cfg" /mnt/boot/grub/
if [ $? -ne 0 ]; then
	exit -8
fi
echo "Copy kernel binary to disk.img"
sudo cp "$OUTPUTTARGET" /mnt/boot/
if [ $? -ne 0 ]; then
	exit -8
fi

sync

sudo umount /mnt
if [ $? -ne 0 ]; then
	exit -8
fi
sudo losetup -d "$LD1"
sudo losetup -d "$LD0"
fi

#rm -f *.o
#rm -rf "$ISODIR"



# # Creating disk image
# # Setup two loop devices. One will be used for writing GRUB and its 
# # additional codes to MBR, and the second will be used for mounting 
# # filesystem of your operating system.
# sudo losetup /dev/loop0 "$OUTPUTTARGET"
# # -o option defines offset from start of the file. Number 1048576 is 
# # actually 1024^2 = 1MB and that's the start of your partition.
# sudo losetup /dev/loop1 "$OUTPUTTARGET" -o 1048576
# # Format your partition
# sudo mke2fs /dev/loop1
# # Mount your newly formatted partition
# sudo mount /dev/loop1 /mnt
# # Install GRUB using grub-install
# sudo grub-install --root-directory=/mnt --no-floppy --modules="normal part_msdos ext2 multiboot biosdev" /dev/loop0
# # Flush the filesystem
# sync
# # Unmount 
# sudo umount /mnt

# mkdir -p "$ISOGRUB"
# cp "$OUTPUTTARGET" "$ISOBOOT"/shos.bin
# cp "$SRCDIR"/grub.cfg "$ISOGRUB"/grub.cfg

# grub-mkrescue -o "$OUTPUTDIR"/simpleos.iso "$ISODIR"
# if [ $? -ne 0 ]; then
# 	exit
# fi
echo Done.