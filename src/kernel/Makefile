NASM=nasm
GCC=i686-elf-gcc

ROOT := ~/projects/qios
OBJDIR := $(ROOT)/obj
SRCDIR := $(ROOT)/src
INC := $(SRCDIR)/include

SRC := $(wildcard *.asm)
CSRC := $(wildcard *.c)
OBJ := $(patsubst %.asm,$(OBJDIR)/%.o,$(SRC))
OBJ += $(patsubst %.c,$(OBJDIR)/%.o,$(CSRC))
ASFLAGS = -felf32 -g
CFLAGS=-std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel -g -O0 --verbose -I $(INC)

.PHONY: all clean

all : $(OBJ)

$(OBJDIR)/%.o : %.asm
	$(NASM) $(ASFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.c
	$(GCC) -c $< -o $@ $(CFLAGS)

debug:
	echo $(SRC)
	echo $(Obj)
clean:
	rm -f $(OBJ)