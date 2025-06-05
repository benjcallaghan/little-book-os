#include "fb.h"
#include "serial.h"
#include "segmentation.h"
#include "interrupts.h"
#include "pic.h"
#include "keyboard.h"
#include "multiboot.h"
#include "printf.h"

int kmain(uint32_t bootloader_magic, multiboot_info_t const *boot_info)
{
    if (bootloader_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        return 1;
    }

    serial_init_com1();
    initialze_segmentation();

    int error = initialize_keyboard();
    if (error)
    {
        return error;
    }

    pic_initialze();
    initialze_interrupts();

    fb_clear();
    fb_write("Hello, world!");
    printf(fb_write_char, "Bootloader flags %X", boot_info->flags);

    serial_write("This is a longer log written to the serial log file.\n");
    serial_write("ASCII control characters like LF should be usable here.\n");

    return 0xCAFEBABE;
}