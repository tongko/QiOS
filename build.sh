#!/bin/bash
set -e

if [[ $# -ne 1 ]] ;
then
	echo
	echo "Usage: build.sh OPTION"
	echo "OPTION:"
	echo "    clean     - Clean all the binaries"
	echo "    install   - Build the kernel"
	echo "    headers   - Install header only"
	echo "    iso       - Build ISO image"
	echo "    qemu      - Start the qemu with kernel image"
	echo

	exit 1
fi

#	Used by functions to return value
RETURN=

# Target triplet to architecture
function fnTargetTripletToArch() {
	RETURN=
	if echo "$1" | grep -Eq 'i[[:digit:]]86-'; then
		RETURN=x86_64
	else
		RETURN="$1" | grep -Eo '^[[:alnum:]_]*'
	fi
}

function fnClean {
	for PROJECT in $PROJECTS; do
		(cd $PROJECT && $MAKE clean)
	done

	rm -rf sysroot
	rm -rf isodir
	rm -rf myos.iso
}

function fnInstallHeaders {
	mkdir -p "$SYSROOT"
	
	for PROJECT in $SYSTEM_HEADER_PROJECTS; do
		(cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers)
	done
}

function fnBuild() {
	fnInstallHeaders

	for PROJECT in $PROJECTS; do
		(cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install)
	done
}

function fnISO() {
	fnBuild
	
	mkdir -p isodir/boot/grub

	cp sysroot/boot/$KERNEL_NAME.kernel isodir/boot/$KERNEL_NAME.kernel
	cat > isodir/boot/grub/grub.cfg << EOF
		menuentry "${KERNEL_NAME}" {
		multiboot /boot/${KERNEL_NAME}.kernel
	}
EOF

	grub-mkrescue -o $KERNEL_NAME.iso isodir
}

function fnQemu() {
	fnISO
	fnTargetTripletToArch $HOST
	qemu-system-$RETURN -cdrom $KERNEL_NAME.iso
}


KERNEL_NAME="QiOS"
SYSTEM_HEADER_PROJECTS="libc hal kernel"
PROJECTS="libc hal kernel"

export MAKE=${MAKE:-make}
export HOST=x86_64-elf

export AR=${HOST}-ar
export AS=nasm
export CC=${HOST}-g++

export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export BOOTDIR=/boot
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

export CFLAGS='-O2 -g'
export CPPFLAGS='-fno-rtti -fno-exceptions'

# Configure the cross-compiler to use the desired system root.
export SYSROOT="$(pwd)/sysroot"
export CC="$CC --sysroot=$SYSROOT"

# Work around that the -elf gcc targets doesn't have a system include directory
# because it was configured with --without-headers rather than --with-sysroot.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi

if [[ $1 == "clean" ]] ;
then
	fnClean
	exit
fi

if [[ $1 == "install" ]] ;
then
	fnBuild
	exit
fi

if [[ $1 == "iso" ]] ;
then
	fnISO
	exit
fi

if [[ $1 == "qemu" ]] ;
then
	fnQemu
	exit
fi