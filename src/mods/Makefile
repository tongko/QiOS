GCC=i686-elf-gcc

ROOT := ~/projects/qios
BIN := $(ROOT)/bin
SRCDIR := $(ROOT)/src/mods
OBJDIR := $(SRCDIR)/obj
INC := $(ROOT)/src/include

CSRC := $(wildcard *.c)
OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(CSRC))
CFLAGS=-std=c11 -ffreestanding -fpic -Wall -Wextra -Werror -masm=intel -g -O0 -I $(INC)
LDFLAGS=-ffreestanding -O0 -nostdlib -lgcc --verbose

.PHONY: all clean

all : $(BIN)/mymodule.so

$(OBJDIR)/%.o : %.c
	$(GCC) -c $< -o $@ $(CFLAGS)

$(BIN)/mymodule.so : $(OBJ)
	$(GCC) -shared -T ModLink.ld -o $@ $(LDFLAGS) $(OBJ) $(ROOT)/obj/term.o $(ROOT)/obj/stdio.o $(ROOT)/obj/string.o $(ROOT)/obj/stdlib.o 

debug:
	echo $(SRC)
	echo $(Obj)
clean:
	rm -f $(OBJ)