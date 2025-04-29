unsigned short *fb = (unsigned short *) 0x000B8000; // The memory-mapped I/O address of the framebuffer.

#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_BROWN 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

/** fb_write_cell:
 * Writes a character with the given foreground and background to position i in the framebuffer.
 * @param i The location in the framebuffer
 * @param c The character
 * @param fg The foreground color 
 * @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
  fb[i] = (bg << 12) | (fg << 8) | c; // Little-endian byte order: [BG/FG, C]
}

int kmain() {
  fb_write_cell(0, 'H', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(1, 'e', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(2, 'l', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(3, 'l', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(4, 'o', FB_GREEN, FB_DARK_GREY);
  return 0xCAFEBABE;
}