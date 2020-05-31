DEFAULT_HOST!=../default-host.sh
HOST?=$(DEFAULT_HOST)
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)

OBJDUMP		:= $(HOST)-objdump
READELF		:= $(HOST)-readelf
OBJCOPY		:= $(HOST)-objcopy

CFLAGS?=-O0 -g
CPPFLAGS?=
LDFLAGS?=
ASFLAGS?= -f elf64 -F dwarf -g -w+all -Werror -w+orphan-labels 
#LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -masm=intel 
CPPFLAGS:=$(CPPFLAGS) -std=c++17 -nostdlib -fno-rtti -fno-exceptions -Iinclude -I../hal/include -D__is_kernel__

ARCHDIR=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS) \
qkernel/QKernel.o \
system/QTerm.o \
system/CopyMemory.o \
system/MemLength.o \
system/SetMemory.o \

OBJS=\
$(ARCHDIR)/crti.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtn.o \

LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/extern/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/extern/crtend.o \
$(ARCHDIR)/crtn.o \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .cpp .asm

all: qios.kernel

qios.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	@printf "\t\e[32;1mLinking\e[0m $(KERNEL)\n"
	@$(CC) -T $(ARCHDIR)/linker.ld -o $@ -z max-page-size=0x1000 $(CFLAGS) $(CPPFLAGS) $(LINK_LIST)
	#$(LINK_LIST)
	@$(OBJCOPY) --only-keep-debug $@ $@.sym
	@$(OBJCOPY) --strip-debug $@
	@$(OBJDUMP) -D $@ > $@.dmp
	@grub-file --is-x86-multiboot2 $@

.cpp.o:
	@printf "\t\e[32;1mCompiling\e[0m $<\n"
	@$(CC) -c $< -o $@ -MMD $(CFLAGS) $(CPPFLAGS)

.asm.o:
	@printf "\t\e[32;1mAssembling\e[0m $<\n"
	@$(AS) $(ASFLAGS) -i $(ARCHDIR)/ -MD $(addsuffix .d,$<) $< -o $@

clean:
	rm -f qios.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
	rm -f $(OBJS:.o=.dep) *.dep */*.dep */*/*.dep
	rm -f $(OBJS:.o=.dmp) *.dmp */*.dmp */*/*.dmp
	rm -f $(OBJS:.o=.elf) *.elf */*.elf */*/*.elf

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: qios.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp qios.kernel* $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
