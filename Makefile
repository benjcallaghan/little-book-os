rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard,src,*.c *.s)
OBJECTS := $(addsuffix .o,$(basename $(SOURCES)))
CC := gcc
CFLAGS := -m32 -nostdlib -ffreestanding -fno-stack-protector -Wall -Wextra -Werror -std=c2x
LDFLAGS := -T link.ld -melf_i386 -l $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
AS := nasm
ASFLAGS := -f elf

.PHONY: all
all: kernel.elf

kernel.elf: $(OBJECTS) link.ld
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso iso

.PHONY: run
run: os.iso
	bochs -f bochsrc.txt -q
	
.PHONY: clean
clean:
	rm -rf kernel.elf os.iso $(OBJECTS)