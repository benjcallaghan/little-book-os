#pragma once

/**
 * Initializes port COM1 for use with this driver.
 */
void serial_init_com1();

/** write:
 * Writes a string to the framebuffer.
 * @param buf The buffer of characters to write.
 * @param len The length of the buffer
 */
void serial_write(char *buf, unsigned int len);