OBJECTS = loader.o kmain.o io.o fb.o serial.o segmentation.o gdt.o idt.o interrupts.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -std=c2x -mgeneral-regs-only
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

OBJECTFILES = $(addprefix obj/,$(OBJECTS))

.PHONY: all
all: kernel.elf

kernel.elf: obj $(OBJECTFILES)
	ld $(LDFLAGS) $(OBJECTFILES) -o kernel.elf

obj:
	mkdir -p obj

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso iso

.PHONY: run
run: os.iso
	bochs -f bochsrc.txt -q

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

obj/%.o: src/%.s
	$(AS) $(ASFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf obj kernel.elf os.iso