#include "fb.h"

int kmain() {
  fb_clear();
  fb_write("Hello, world!", 13);
  return 0xCAFEBABE;
}