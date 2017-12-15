# GCC=i686-elf-gcc
# CFLAGS=-std=c11 -ffreestanding -Wall -Wextra -Werror -masm=intel -g -O0 -I ../include
# LDFLAGS=-shared -ffreestanding -O2 -nostdlib -lgcc --verbose 

# .PHONY: all clean
# all : ../../obj/string.o ../../obj/stdlib.o ../../obj/stdio.o

# ../../obj/string.o : string.c ../include/string.h ../include/stddef.h
# 	$(GCC) -c string.c -o $@ $(CFLAGS)

# ../../obj/stdlib.o : stdlib.c ../include/stdlib.h ../include/string.h ../include/stddef.h
# 	$(GCC) -c stdlib.c -o $@ $(CFLAGS)

# ../../obj/stdio.o : stdio.c ../include/stdio.h ../include/stddef.h ../include/string.h ../include/stdlib.h ../include/sys/term.h ../include/stdarg.h
# 	$(GCC) -c stdio.c -o $@ $(CFLAGS)

# clean:
# 	rm -f ../../obj/stdio.o ../../obj/stdlib.o ../../obj/string.o
GCC=i686-elf-gcc
ROOT := ~/projects/qios
OBJDIR := $(ROOT)/obj
SRCDIR := $(ROOT)/src
INC := $(SRCDIR)/include

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
