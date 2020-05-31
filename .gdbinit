file sysroot/boot/qios.kernel
symbol-file sysroot/boot/qios.kernel.sym
set substitute-path arch/x86_64 kernel/arch/x86_64
set architecture i386:x86-64
set disassembly-flavor intel
set pagination off
target remote localhost:1234
hbreak *0x100018
hbreak *0x100055
continue
tui enable
