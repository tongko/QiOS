# This file makes use of automatic variables
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables

# https://en.wikipedia.org/wiki/Netwide_Assembler
NASM=nasm

# https://en.wikipedia.org/wiki/QEMU
# Virtual machine
QEMU=qemu-system-i386

GCC=i686-elf-gcc

CFLAGS=-std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel -g -O0 \
			-I ../../../include \
			-funsigned-char 

LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc --verbose 

ASFLAGS=-felf32 -g

OBJ := $(patsubst %.c,obj/%.o,$(wildcard *.c))
OBJ += $(patsubst %.asm,obj/%.o,$(wildcard *.asm))

.PHONY: all clean
all:bin/boot.iso

obj/%.o : %.c
	$(GCC) -c $< -o $@ $(CFLAGS) 

obj/%.o : %.asm
	$(NASM) $(ASFLAGS) -o $@ $<

bin/kernel.elf : $(OBJ)
	$(GCC) -T link.ld -o $@ $(LDFLAGS) $^ # Link to make an executable for the kernel.
	grub-file --is-x86-multiboot2 $@

bin/boot.iso : bin/kernel.elf grub.cfg
	mkdir -p bin/iso/boot/grub              # create the folder structure
	cp bin/kernel.elf bin/iso/boot/             # copy the kernel
	cp grub.cfg bin/iso/boot/grub           # copy the grub configuration file
	grub-mkrescue -o $@ bin/iso

run: bin/boot.iso
	# view contents of register with `p $$eax`
	$(QEMU) -cdrom $< -D log/qemu.log -d guest_errors -m 2048M -daemonize -serial file:log/boot.log

debug: bin/boot.iso bin/kernel.elf
	$(QEMU) -cdrom $< -D log/qemu.log -d guest_errors -m 2048M -daemonize -serial file:log/boot.log -s -S
	gdb bin/kernel.elf -ex "target remote :1234" --tui

clean:
	rm -f *.dmp *.objdump *.log
	rm -rf obj/*
	rm -rf bin/*
