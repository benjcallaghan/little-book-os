#pragma once

/**
 * Initializes port COM1 for use with this driver.
 */
void serial_init_com1();

/** write:
 * Writes a string to the framebuffer.
 * @param str A null-delimited string.
 */
void serial_write(char const *str);