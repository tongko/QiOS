add-auto-load-safe-path /home/tongko/projects/qios/.gdbinit
file ../.sysroot/boot/qios.kernel
symbol-file ../.sysroot/boot/qios.kernel.sym
directory /home/tongko/projects/qios/src/kernel
directory /home/tongko/projects/qios/src/kernel/arch/x86_64
directory /home/tongko/projects/qios/src/kernel/mm
directory /home/tongko/projects/qios/src/kernel/runtime
directory /home/tongko/projects/qios/src/kernel/system
directory /home/tongko/projects/qios/src/klib
directory /home/tongko/projects/qios/src/klib/arch/x86_64
directory /home/tongko/projects/qios/src/klib/Mutex
directory /home/tongko/projects/qios/src/klib/stdio
directory /home/tongko/projects/qios/src/klib/stdlib
directory /home/tongko/projects/qios/src/klib/string
set substitute-path arch/x86_64 kernel/arch/x86_64
set disassembly-flavor intel
set pagination off
set architecture i386:x86-64:intel
target remote localhost:1234
break entry.asm:long_mode_entry
break qkernel.cpp:_kmain
continue
