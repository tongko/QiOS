#!/bin/bash

LD_VER=$1

export TARGET=x86_64-elf
export PREFIX="/usr/local/opt/cross/$TARGET"
export PATH="$PREFIX/bin:$PATH"

cd /usr/local/src
mkdir build-binutils
cd build-binutils
../binutils-${LD_VER}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j 4
make install

cd /usr/local/src
rm -rf build-binutils.sh build-binutils binutils-${LD_VER}