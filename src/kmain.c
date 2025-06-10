#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "segmentation.h"
#include "interrupts.h"
#include "drivers/pic.h"
#include "drivers/keyboard.h"
#include "multiboot.h"
#include "printf.h"

constexpr uint32_t MULTIBOOT_FLAGS = MULTIBOOT_PAGE_ALIGN;
const struct multiboot_header header __attribute__((section(".multiboot"))) = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_FLAGS,
    .checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS),
};

typedef uint32_t (*call_module_t)(void);

int kmain(uint32_t bootloader_magic, struct multiboot_info const *boot_info)
{
    if (bootloader_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        return 1;
    }

    serial_init_com1();
    segmentation_initialize();

    int error = keyboard_initialze();
    if (error)
    {
        return error;
    }

    pic_initialze();
    interrupts_initialize();

    framebuffer_clear();
    framebuffer_write("Hello, world!\n");
    printf(framebuffer_write_char, "Bootloader flags %X\n", boot_info->flags);

    serial_write("This is a longer log written to the serial log file.\n");
    serial_write("ASCII control characters like LF should be usable here.\n");

    if ((boot_info->flags & MULTIBOOT_INFO_MODS) && boot_info->mods_count > 0)
    {
        printf(serial_write_char, "Number of boot modules %X\n", boot_info->mods_count);
        printf(serial_write_char, "Address of module structures %X\n", boot_info->mods_addr);
        struct multiboot_mod_list *modules = (struct multiboot_mod_list *)boot_info->mods_addr;

        printf(serial_write_char, "Address of start of module %X\n", modules[0].mod_start);
        call_module_t program = (call_module_t)modules[0].mod_start;
        uint32_t result = program();
        printf(serial_write_char, "RESULT %X", result);
        printf(framebuffer_write_char, "RESULT %X\n", result);
    }

    return 0xCAFEBABE;
}