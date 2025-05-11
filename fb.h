#pragma once

/** write:
 * Writes a string to the framebuffer.
 * @param str A null-delimited string.
 */
void fb_write(char const *str);

/**
 * Clears the framebuffer of all contents
 */
void fb_clear();