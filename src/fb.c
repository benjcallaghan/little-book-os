#include "io.h"
#include <stddef.h>
#include <stdint.h>

enum fb_color
{
    black = 0,
    blue = 1,
    green = 2,
    cyan = 3,
    red = 4,
    magenta = 5,
    brown = 6,
    light_grey = 7,
    dark_grey = 8,
    light_blue = 9,
    light_green = 10,
    light_cyan = 11,
    light_red = 12,
    light_magenta = 13,
    light_brown = 14,
    white = 15,
};

struct fb_cell
{
    char character : 8;
    enum fb_color foreground_color : 4;
    enum fb_color background_color : 4;
} __attribute__((packed));
const struct fb_cell null_cell = {.character = 0, .foreground_color = 0, .background_color = 0};

struct fb_cell *const fb = (struct fb_cell *)0x000B8000; // The memory-mapped I/O address of the framebuffer.

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FB_COLS 80
#define FB_ROWS 25
#define FB_CELLS (FB_COLS * FB_ROWS)

uint16_t cursor_pos = 0;

/** fb_move_cursor:
 * Moves the cursor of the framebuffer to the given position
 * @param pos The new position of the cursor
 */
void fb_move_cursor(uint16_t pos)
{
    if (pos > FB_CELLS)
    {
        // Scroll all text up one line
        for (size_t i = 0; i < FB_CELLS - FB_COLS; i++)
        {
            fb[i] = fb[i + FB_COLS];
        }

        // Clear the final line
        for (size_t i = FB_CELLS - FB_COLS; i < FB_CELLS; i++)
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

void fb_write_char(char c)
{
    fb[cursor_pos] = (struct fb_cell){.character = c, .foreground_color = white, .background_color = black};
    fb_move_cursor(cursor_pos + 1);
}

void fb_write(char const *str)
{
    char c;
    while ((c = *str++))
    {
        fb_write_char(c);
    }
}

void fb_clear()
{
    for (size_t i = 0; i < FB_CELLS; i++)
    {
        fb[i] = null_cell;
    }
    fb_move_cursor(0);
}
