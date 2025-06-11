#include "logger.h"
#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "segmentation.h"
#include "interrupts.h"
#include "drivers/pic.h"
#include "drivers/keyboard.h"
#include "multiboot.h"
#include "printf.h"

constexpr uint32_t MULTIBOOT_FLAGS = MULTIBOOT_PAGE_ALIGN | MULTIBOOT_VIDEO_MODE;
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

    framebuffer_clear();
    serial_init_com1();
    
    segmentation_initialize();

    int error = keyboard_initialze();
    if (error)
    {
        return error;
    }

    pic_initialze();
    interrupts_initialize();

    logf(log_info, "Hello, world!");    
    logf(log_debug, "Bootloader flags %X", boot_info->flags);

    if (boot_info->flags & MULTIBOOT_INFO_CMDLINE)
    {
	char const *cmdline = (char const *)boot_info->cmdline;
    	logf(log_info, "Kernel booted with %s", cmdline);
    }

    if (boot_info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME)
    {
	    char const *bootloader = (char const *)boot_info->boot_loader_name;
	    logf(log_info, "Kernel booted by %s", bootloader);
    }
    
    if ((boot_info->flags & MULTIBOOT_INFO_MODS) && boot_info->mods_count > 0)
    {
        logf(log_debug, "Number of boot modules %X", boot_info->mods_count);
        logf(log_debug, "Address of module structures %X", boot_info->mods_addr);
        struct multiboot_mod_list *modules = (struct multiboot_mod_list *)boot_info->mods_addr;

        logf(log_debug, "Address of start of module %X", modules[0].mod_start);
        call_module_t program = (call_module_t)modules[0].mod_start;
        uint32_t result = program();
        logf(log_info, "Boot Module Result: %X", result);
    }

    return 0xCAFEBABE;
}
