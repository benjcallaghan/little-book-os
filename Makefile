OBJECTS = $(addprefix obj/,loader.o kmain.o io.o fb.o serial.o segmentation.o gdt.o idt.o interrupts.o printf.o pic.o)
CC = gcc
CFLAGS = -m32 -nostdlib -ffreestanding -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -std=c2x
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

.PHONY: all
all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso iso

.PHONY: run
run: os.iso
	bochs -f bochsrc.txt -q

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) $< -o $@

obj/%.o: src/%.s | obj
	$(AS) $(ASFLAGS) $< -o $@

obj:
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf obj kernel.elf os.iso