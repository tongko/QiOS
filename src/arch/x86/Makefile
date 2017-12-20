NASM=nasm
GCC=i686-elf-gcc

ROOT := ~/projects/qios
OBJDIR := $(ROOT)/obj
SRCDIR := $(ROOT)/src
INC := $(SRCDIR)/include
X86 := $(SRCDIR)/arch/x86

SRC := $(wildcard *.asm)
CSRC := $(wildcard *.c)
OBJ := $(patsubst %.asm,$(OBJDIR)/%.o,$(SRC))
OBJ += $(patsubst %.c,$(OBJDIR)/%.o,$(CSRC))
ASFLAGS = -felf32 -g
CFLAGS=-std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel -g -O0 -I $(INC)

.PHONY: all clean
all: $(OBJ) subsystem

subsystem:
	+$(MAKE) -C $(X86)/comm
	+$(MAKE) -C $(X86)/hal
	+$(MAKE) -C $(X86)/boot

$(OBJDIR)/%.o : %.asm
	$(NASM) $(ASFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.c
	$(GCC) -c $< -o $@ $(CFLAGS)

debug:
	@echo $(OBJ)

clean:
	+$(MAKE) -C $(X86)/comm clean
	+$(MAKE) -C $(X86)/hal clean
	+$(MAKE) -C $(X86)/boot clean
	rm -f $(OBJ)
