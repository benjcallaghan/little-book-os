#include "fb.h"
#include "serial.h"
#include "segmentation.h"
#include "interrupts.h"
#include "keyboard.h"

int kmain()
{
    serial_init_com1();    
    initialze_segmentation();
    initialze_interrupts();
    
    int result = initialize_keyboard();
    if (!result) {
        return result;
    }

    fb_clear();
    fb_write("Hello, world!");
    fb_write("This");
    fb_write("Is");
    fb_write("A");
    fb_write("Test");

    serial_write("This is a longer log written to the serial log file.\n");
    serial_write("ASCII control characters like LF should be usable here.\n");

    return 0xCAFEBABE;
}