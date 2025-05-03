#include "fb.h"
#include "serial.h"

int kmain() {
  fb_clear();
  fb_write("Hello, world!", 13);
  fb_write("This", 4);
  fb_write("Is", 2);
  fb_write("A", 1);
  fb_write("Test", 4);

  serial_init_com1();
  int err = serial_write("This is a longer log written to the serial log file.\n", 53);
  if (err) {
    fb_write("Error in serial write: ", 23);
    char errc = '0' + err;
    fb_write(&errc, 1);
  }

  err = serial_write("ASCII control characters like LF should be usable here.\n", 56);
  if (err) {
    fb_write("Error in serial write: ", 23);
    char errc = '0' + err;
    fb_write(&errc, 1);
  }
  
  return 0xCAFEBABE;
}