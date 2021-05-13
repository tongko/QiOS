#!/bin/bash

GCC_VER=$1

export TARGET=x86_64-elf
export PREFIX="$HOME/opt/cross/$TARGET"
export PATH="$PREFIX/bin:$PATH"

# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH

#Download prerequisites
cd $HOME/src
$HOME/src/gcc-${GCC_VER}/contrib/download_prerequisites

#Patch with no-red-zone
cd $HOME/src/gcc-${GCC_VER}/gcc
patch < config.gcc.patch

cd $HOME/src

mkdir build-gcc
cd build-gcc
../gcc-${GCC_VER}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j 4 all-gcc
make -j 4 all-target-libgcc CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone'
make install-gcc
make install-target-libgcc

cd $HOME/src
rm -rf build-gcc.sh build-gcc gcc-${GCC_VER}