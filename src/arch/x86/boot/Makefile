# This file makes use of automatic variables
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables

# https://en.wikipedia.org/wiki/Netwide_Assembler
NASM=nasm

# https://en.wikipedia.org/wiki/QEMU
# Virtual machine
QEMU=qemu-system-i386

GCC=i686-elf-gcc

CFLAGS=-std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel -g -O0

ASFLAGS=-felf32 -g

OBJ := $(patsubst %.c,%.o,$(wildcard *.c))
OBJ += $(patsubst %.asm,%.o,$(wildcard *.asm))

.PHONY: all clean
all:boot.iso

%.o : %.c
	$(GCC) -c $< $(CFLAGS) -o $@

%.o : %.asm
	$(NASM) $(ASFLAGS) -o $@ $<

kernel.elf : $(OBJ)
	$(GCC) -T link.ld -o $@ -ffreestanding -O0 -nostdlib -lgcc --verbose $^ # Link to make an executable for the kernel.
	grub-file --is-x86-multiboot2 $@

boot.iso : kernel.elf grub.cfg
	mkdir -p ./iso/boot/grub              # create the folder structure
	cp kernel.elf ./iso/boot/             # copy the kernel
	cp grub.cfg ./iso/boot/grub           # copy the grub configuration file
	grub-mkrescue -o $@ ./iso

run: boot.iso
	# view contents of register with `p $$eax`
	$(QEMU) -cdrom $< -D log/qemu.log -d guest_errors -m 2048M -daemonize -serial file:log/boot.log

debug: boot.iso
	$(QEMU) -cdrom $< -D log/qemu.log -d guest_errors -m 2048M -daemonize -serial file:log/boot.log -s -S
	gdb ./kernel.elf -ex "target remote :1234" --tui

clean:
	rm -f ./*.o
	rm -f ./kernel.elf
	rm -rf ./iso
