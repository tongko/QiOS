GCC=i686-elf-gcc
ROOT := ~/projects/qios
OBJDIR := $(ROOT)/obj
SRCDIR := $(ROOT)/src
INC := $(SRCDIR)/include
X86 := $(SRCDIR)/arch/x86

CFLAGS=-std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel -g -O0 -I $(INC)
CSRC = $(wildcard *.c)
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(CSRC))

.PHONY: all clean debug
all: $(OBJ)

$(OBJDIR)/%.o : %.c
	$(GCC) -c $< -o $@ $(CFLAGS)

debug:
	@echo $(OBJ)

clean:
	rm -f $(OBJ)
