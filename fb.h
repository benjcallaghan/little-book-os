#pragma once

/** write:
 * Writes a string to the framebuffer.
 * @param buf The buffer of characters to write.
 * @param len The length of the buffer
 */
int fb_write(char *buf, unsigned int len);

/**
 * Clears the framebuffer of all contents
 */
void fb_clear();