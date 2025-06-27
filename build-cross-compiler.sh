#!/bin/bash
export PREFIX="`pwd`/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# mkdir -p build-binutils
# cd build-binutils
# ../binutils-gdb/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
# make
# make install

# cd ..
mkdir -p build-gcc
cd build-gcc
../gcc/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx
make all-gcc
make all-target-libgcc
make all-target-libstdc++-v3
make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3