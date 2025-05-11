#include "io.h"

struct fb_cell {
    char character: 8;
    unsigned char foreground_color: 4;
    unsigned char background_color: 4;
};
struct fb_cell null_cell = { .character = 0, .foreground_color = 0, .background_color = 0 };

struct fb_cell *fb = (struct fb_cell *)0x000B8000; // The memory-mapped I/O address of the framebuffer.

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

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FB_COLS 80
#define FB_ROWS 25
#define FB_CELLS (FB_COLS * FB_ROWS)

unsigned short cursor_pos = 0;

/** fb_move_cursor:
 * Moves the cursor of the framebuffer to the given position
 * @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos)
{
    if (pos > FB_CELLS)
    {
        // Scroll all text up one line
        for (unsigned int i = 0; i < FB_CELLS - FB_COLS; i++)
        {
            fb[i] = fb[i + FB_COLS];
        }

        // Clear the final line
        for (unsigned int i = FB_CELLS - FB_COLS; i < FB_CELLS; i++)
        {
            fb[i] = null_cell;
        }

        pos -= FB_COLS; // Rewind one row
    }

    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos >> 8);
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0xFF);
    cursor_pos = pos;
}

void fb_write(char *buf, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        fb[cursor_pos] = (struct fb_cell){ .character = buf[i], .foreground_color = FB_WHITE, .background_color = FB_BLACK };
        fb_move_cursor(cursor_pos + 1);
    }
}

void fb_clear()
{
    for (unsigned int i = 0; i < FB_CELLS; i++)
    {
        fb[i] = null_cell;
    }
    fb_move_cursor(0);
}
