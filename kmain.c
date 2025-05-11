#include "fb.h"
#include "serial.h"
#include "segmentation.h"

int kmain()
{
    initialze_segmentation();

    fb_clear();
    fb_write("Hello, world!");
    fb_write("This");
    fb_write("Is");
    fb_write("A");
    fb_write("Test");

    serial_init_com1();
    serial_write("This is a longer log written to the serial log file.\n", 53);
    serial_write("ASCII control characters like LF should be usable here.\n", 56);

    return 0xCAFEBABE;
}