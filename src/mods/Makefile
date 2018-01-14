GCC=i686-elf-gcc

ROOT := ~/projects/qios
BIN := $(ROOT)/bin
SRCDIR := $(ROOT)/src/mods
OBJDIR := $(SRCDIR)/obj
INC := $(ROOT)/src/include

CSRC := $(wildcard *.c)
OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(CSRC))
CFLAGS=-std=c11 -Wall -Wextra -Werror -masm=intel -g -O0 -fPIC -ffreestanding -I $(INC)
LDFLAGS=-O0 -nostdlib -shared -fPIC -Wl,-soname,mymodule.so -ffreestanding 

.PHONY: all clean

all : $(BIN)/mymodule.so

$(OBJDIR)/%.o : %.c
	$(GCC) -c $< -o $@ $(CFLAGS)

$(BIN)/mymodule.so : $(OBJ)
	$(GCC) -o $@ $(LDFLAGS) $(OBJ)
	@objdump -D -Mintel $@ > $(BIN)/mod.dump
	@readelf -a $@ > $(BIN)/mod.elfh

debug:
	echo $(SRC)
	echo $(Obj)
clean:
	rm -f $(OBJ)