#include "fb.h"
#include "serial.h"
#include "segmentation.h"
#include "interrupts.h"
#include "pic.h"
#include "keyboard.h"

int kmain()
{
    serial_init_com1();    
    initialze_segmentation();
    
    int error = initialize_keyboard();
    if (error) {
        return error;
    }

    pic_initialze();
    initialze_interrupts();

    fb_clear();
    fb_write("Hello, world!");

    serial_write("This is a longer log written to the serial log file.\n");
    serial_write("ASCII control characters like LF should be usable here.\n");

    return 0xCAFEBABE;
}