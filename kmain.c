#include "fb.h"
#include "serial.h"
#include "segmentation.h"

int kmain()
{
  initialze_segmentation();

  fb_clear();
  fb_write("Hello, world!", 13);
  fb_write("This", 4);
  fb_write("Is", 2);
  fb_write("A", 1);
  fb_write("Test", 4);

  serial_init_com1();
  serial_write("This is a longer log written to the serial log file.\n", 53);
  serial_write("ASCII control characters like LF should be usable here.\n", 56);

  return 0xCAFEBABE;
}