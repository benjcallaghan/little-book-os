#include "fb.h"

int kmain() {
  fb_clear();
  fb_write("Hello, world!", 13);
  fb_write("This", 4);
  fb_write("Is", 2);
  fb_write("A", 1);
  fb_write("Test", 4);
  fb_write("ReallyLooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooongText", 120);
  return 0xCAFEBABE;
}