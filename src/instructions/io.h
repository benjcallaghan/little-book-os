#pragma once
#include <stdint.h>

/** outb:
 * Sends the given data to the given I/O port. Defined in io.s
 * @param port the I/O port to send the data to
 * @param data The data to send to the I/O port
 */
void outb(uint16_t port, uint8_t data);

/**
 * Read a byte from an I/O port
 * @param port The address of the I/O port
 * @return The read byte
 */
uint8_t inb(uint16_t port);