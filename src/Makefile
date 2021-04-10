default_host ?= $(shell ./default-host.sh)
HOST ?= $(default_host)
HOSTARCH ?= $(shell ./target-triplet-to-arch.sh $(HOST))

PREFIX?=/usr
BOOTDIR?=/boot
LIBDIR?=$(PREFIX)/lib
INCLUDEDIR?=$(PREFIX)/include

AS = nasm
CC = $(HOST)-g++

CFLAGS = -O0 -g
CPPFLAGS =
ASFLAGS = -F dwarf -g -w+all -Werror -w+orphan-labels 

# Configure the cross-compiler to use the desired system root.
pwd = $(shell pwd)
sysroot = $(pwd)/../.sysroot
output_dir := $(pwd)/../.obj

$(shell mkdir -p $(output_dir))

CFLAGS += --sysroot=$(sysroot)

# Work around that the -elf gcc targets doesn't have a system include directory
# because it was configured with --without-headers rather than --with-sysroot.
CFLAGS += -isystem=$(INCLUDEDIR) -ffreestanding -Wall -Wextra -masm=intel -mgeneral-regs-only
CPPFLAGS += -std=c++17 -nostdlib -fno-rtti -fno-exceptions -I$(pwd)/include -D__is_kernel__ -z max-page-size=0x1000 -fno-sized-deallocation 

LDFLAGS = -nostdlib 

DESTDIR?=$(sysroot)

iso-dir = $(pwd)/../.isodir
objcopy = $(HOST)-objcopy
objdump = $(HOST)-objdump
kernel = qios.kernel

export

arch_dir = ./arch/$(HOSTARCH)
include $(arch_dir)/make.config
CFLAGS += $(KERNEL_ARCH_CFLAGS)
CPPFLAGS += $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS += $(KERNEL_ARCH_LDFLAGS)

#arch_dir = ./kernel/arch/$(HOSTARCH)
#LDFLAGS := $(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)

link_objs = \
$(output_dir)/crti.o \
$(shell $(CC) $(CFLAGS) $(CPPFLAGS) -print-file-name=crtbegin.o) \
$(shell find $(output_dir) -name "*.o" ! -name "*crt*.o") \
$(output_dir)/libklib.a \
$(shell $(CC) $(CFLAGS) $(CPPFLAGS) -print-file-name=crtend.o) \
$(output_dir)/crtn.o

VARS_OLD := $(.VARIABLES)

.PHONY: all qios.iso
all: qios.iso

qios.iso: build

.PHONY: build headers compile printobj

printobj:
	@printf "$(CC) $(CFLAGS) $(CPPFLAGS) -print-file-name=crtbegin.o \n"
	@printf "$(shell $(CC) $(CFLAGS) $(CPPFLAGS) -print-file-name=crtbegin.o) \n"

build: $(DESTDIR)$(BOOTDIR) headers compile $(arch_dir)/linker.ld
	@printf "\e[32;7mLinking QKernel binary...\e[0m\n"
	# @printf "\n\n $(LDFLAGS) -o $(output_dir)/$(kernel) $(CFLAGS) $(CPPFLAGS) $(link_objs) -lgcc \n\n"
	$(CC) -T $(arch_dir)/linker.ld $(LDFLAGS) -o $(output_dir)/$(kernel) $(CFLAGS) $(CPPFLAGS) $(link_objs) -lgcc
	$(objcopy) --only-keep-debug $(output_dir)/$(kernel) $(output_dir)/$(kernel).sym
	#@$(objcopy) --strip-debug $(output_dir)/$(kernel)
	$(objdump) -D  $(output_dir)/$(kernel) > $(output_dir)/$(kernel).dmp
	grub-file --is-x86-multiboot2 $(output_dir)/$(kernel)
	cp $(output_dir)/$(kernel)* $(DESTDIR)$(BOOTDIR)
	@printf "\e[32;7mAll done!\e[0m\n"

$(DESTDIR)$(BOOTDIR):
	@printf "Make directory \e[32;3m$(DESTDIR)$(BOOTDIR)\e[0m\n"
	@mkdir -p $(DESTDIR)$(BOOTDIR)

headers: $(DESTDIR)$(INCLUDEDIR)
	@printf "Copy headers to \e[32;3m$(DESTDIR)$(INCLUDEDIR)...\e[0m\n"
	@cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

$(DESTDIR)$(INCLUDEDIR):
	@printf "Make directory \e[32;3m$(DESTDIR)$(INCLUDEDIR)...\e[0m\n"
	@mkdir -p $(DESTDIR)$(INCLUDEDIR)

.PHONY: $(output_dir) klib kernel

compile: $(output_dir) klib kernel
	@printf "\n\e[32mCompiling done\e[0m\n\n"

klib:
	@printf "\n\e[32mBegin compiling Kernel Framework Library\e[0m\n"
	@$(MAKE) -C ./klib
	#@$(MAKE) --debug=a -C ./klib

kernel:
	@printf "\n\e[32mBegin compiling QKernel\e[0m\n"
	@$(MAKE) -C ./kernel
	#@$(MAKE) --debug=a -C ./kernel

.PHONY: clean build iso qemu run

rebuild: clean build
	@printf "\e[32;7mBuild all\e[0m\n"

debug: build iso qemu
	@ddd -iex "set auto-load safe-path /"

clean:
	@printf "\e[32mCleaning...\e[0m $<\n"
	@rm -rf $(output_dir)
	@rm -rf $(DESTDIR)
	@rm -rf $(iso-dir)
	@rm -rf qios.iso qios.kernel.*

qemu:
	@printf "\e[32;1mStarting kernel with qemu\e[0m\n"
	@qemu-system-$(HOSTARCH) -S -gdb tcp::1234 -boot d -cdrom qios.iso -no-reboot -m 4096M -d guest_errors,int,cpu_reset -smp 1 -usb -vga std -serial file:../.logs/com1.log -D ../.logs/qemu.log

run: build iso
	@qemu-system-$(HOSTARCH) -qmp tcp:localhost:4444,server,nowait -S -gdb tcp::1234 -boot d -cdrom qios.iso -no-reboot -m 4096M -d guest_errors,int,cpu_reset -smp 1 -usb -vga std -serial file:../.logs/com1.log -D ../.logs/qemu.log

.ONESHELL:
iso:
	@printf "\e[32;1mMake ISO image\e[0m\n"
	mkdir -p $(iso-dir)/boot/grub
	cp $(DESTDIR)$(BOOTDIR)/$(kernel) $(iso-dir)/boot/$(kernel)
	cat <<- EOF > $(iso-dir)/boot/grub/grub.cfg
	menuentry "QiOS" {
		multiboot2 /boot/$(kernel)
	}
	EOF
	grub-mkrescue -o qios.iso $(iso-dir)
	mkdir -p ../.logs
