#include "io.h"

/*
 * All I/O ports are calculated relative to the data port. This is because all serial ports
 * (COM1, COM2, COM3, COM4) have their ports in the same order, but they start at different values.
 */
static constexpr uint16_t SERIAL_COM1_BASE = 0x03F8;

#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_DIVISOR_PORT_LOW(base) (base)
#define SERIAL_DIVISOR_PORT_HIGH(base) (base + 1)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

// Tells the serial port to expect first the highest 8 bits on the data port, then the lowest 8 bits will follow.
static constexpr uint8_t SERIAL_LINE_ENABLE_DLAB = 0x80;

/**
 * Sets the speed of the data being sent. The default speed of a serial port is 115200 bits/s.
 * The argument is a divisor of that number, hence the resulting speed becomes (115200 / divisor) bits/s.
 * @param com The COM port to configure
 * @param divisor The divisor
 */
static void serial_configure_baud_rate(uint16_t com, int divisor)
{
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DIVISOR_PORT_HIGH(com), divisor >> 8);
    outb(SERIAL_DIVISOR_PORT_LOW(com), divisor & 0xFF);
}

/**
 * Configures the line of the given serial port. The port is set to have a data length of 8 bits,
 * no parity bits, one stop bit and break control disabled.
 * @param com The serial port to configure
 */
static void serial_configure_line(uint16_t com)
{
    /*
     * Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
     */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

static void serial_configure_buffers(uint16_t com)
{
    /*
     * Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
     * Content: | lvl | bs | r | dma | clt | clr | e |
     * Value:   | 1 1 | 0  | 0 | 0   | 1   | 1   | 1 | = 0xC7
     */
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

static void serial_configure_modem(uint16_t com)
{
    /*
     * Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
     * Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
     * Value:   | 0 | 0 | 0  | 0  | 0   | 0   | 1   | 1   | = 0x03
     */
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

static bool serial_is_transmit_fifo_empty(uint16_t com)
{
    // 0x20 = Bit 5
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_init_com1()
{
    serial_configure_baud_rate(SERIAL_COM1_BASE, 1);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_buffers(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);
}

void serial_write_char(char c)
{
    while (!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE))
        ;
    outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), c);
}

void serial_write(char const *str)
{
    char c;
    while ((c = *str++))
    {
        serial_write_char(c);
    }
}