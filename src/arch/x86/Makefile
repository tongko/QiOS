GCC=i686-elf-gcc
ROOT := ~/projects/qios
OBJDIR := $(ROOT)/obj
SRCDIR := $(ROOT)/src
INC := $(SRCDIR)/include
X86 := $(SRCDIR)/arch/x86

CFLAGS=-std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel -g -O0 -I $(INC)
CSRC = $(wildcard *.c)
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(CSRC)))

.PHONY: all clean
all: $(OBJ) subsystem

subsystem:
	+$(MAKE) -C $(X86)/comm
	+$(MAKE) -C $(X86)/multiboot
	+$(MAKE) -C $(X86)/boot

$(OBJDIR)/%.o : $(CSRC)
	$(GCC) -c $< -o $@ $(CFLAGS)

debug:
	@echo $(OBJ)

clean:
	+$(MAKE) -C $(X86)/comm clean
	+$(MAKE) -C $(X86)/multiboot clean
	+$(MAKE) -C $(X86)/boot clean
	rm -f $(OBJ)
