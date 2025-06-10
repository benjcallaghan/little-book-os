#pragma once

/**
 * Initializes port COM1 for use with this driver.
 */
void serial_init_com1();

/**
 * Writes a single char to the serial port.
 * @param char The character to write.
 */
void serial_write_char(char c);

/** write:
 * Writes a string to the serial port.
 * @param str A null-delimited string.
 */
void serial_write(char const *str);