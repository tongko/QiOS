#!/bin/bash

LD_VER=$1

export TARGET=x86_64-elf
export PREFIX="$HOME/opt/cross/$TARGET"
export PATH="$PREFIX/bin:$PATH"

cd $HOME/src
mkdir build-binutils
cd build-binutils
../binutils-${LD_VER}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j 4
make install

cd $HOME/src
rm -rf build-binutils.sh build-binutils binutils-${LD_VER}