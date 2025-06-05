#include "fb.h"
#include "serial.h"
#include "segmentation.h"
#include "interrupts.h"
#include "pic.h"
#include "keyboard.h"
#include "multiboot.h"
#include "printf.h"

typedef uint32_t (*call_module_t)(void);

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
    fb_write("Hello, world!\n");
    printf(fb_write_char, "Bootloader flags %X\n", boot_info->flags);

    serial_write("This is a longer log written to the serial log file.\n");
    serial_write("ASCII control characters like LF should be usable here.\n");

    if ((boot_info->flags & MULTIBOOT_INFO_MODS) && boot_info->mods_count > 0)
    {
        printf(serial_write_char, "Number of boot modules %X\n", boot_info->mods_count);
        printf(serial_write_char, "Address of module structures %X\n", boot_info->mods_addr);
        multiboot_module_t *modules = (multiboot_module_t *)boot_info->mods_addr;

        printf(serial_write_char, "Address of start of module %X\n", modules->mod_start);
        call_module_t program = (call_module_t)modules->mod_start;
        uint32_t result = program();
        printf(serial_write_char, "RESULT %X", result);
        printf(fb_write_char, "RESULT %X\n", result);
    }

    return 0xCAFEBABE;
}