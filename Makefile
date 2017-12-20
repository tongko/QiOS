QEMU=qemu-system-i386
GCC=i686-elf-gcc
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc --verbose
OBJ = $(wildcard obj/*.o)

.PHONY: all clean compile bin/kernel.elf bin/boot.iso run debug
all:bin/boot.iso

compile:
	+$(MAKE) -C src/lib
	+$(MAKE) -C src/arch/x86
	+$(MAKE) -C src/kernel

bin/kernel.elf : compile
	# Link to make an executable for the kernel.
	$(GCC) -T src/link.ld -o $@ $(LDFLAGS) $(OBJ)
	grub-file --is-x86-multiboot2 $@

bin/boot.iso : bin/kernel.elf src/grub.cfg
	@mkdir -p bin/iso/boot/grub              # create the folder structure
	@cp bin/kernel.elf bin/iso/boot/             # copy the kernel
	@cp src/grub.cfg bin/iso/boot/grub           # copy the grub configuration file
	@grub-mkrescue -o $@ bin/iso

run: bin/boot.iso
	# view contents of register with `p $$eax`
	$(QEMU) -cdrom $< -D log/qemu.log -d guest_errors -machine q35 -enable-kvm -m 2048M -daemonize -serial file:log/boot.log

debug: bin/boot.iso bin/kernel.elf
	$(QEMU) -cdrom $< -D log/qemu.log -d guest_errors -machine q35 -enable-kvm -m 2048M -daemonize -serial file:log/boot.log -s -S
	gdb bin/kernel.elf -ex "target remote :1234" --tui

clean:
	+$(MAKE) -C src/lib clean
	+$(MAKE) -C src/arch/x86 clean
	+$(MAKE) -C src/kernel clean
	rm -f *.dmp *.objdump *.log
	rm -rf obj/*
	rm -rf bin/*
