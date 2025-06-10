#include <stdint.h>
#include "../io.h"

static constexpr uint16_t PIC1 = 0x20;
static constexpr uint16_t PIC2 = 0xA0;
static constexpr uint16_t PIC1_COMMAND = PIC1;
static constexpr uint16_t PIC1_DATA = (PIC1 + 1);
static constexpr uint16_t PIC2_COMMAND = PIC2;
static constexpr uint16_t PIC2_DATA = (PIC2 + 1);
static constexpr uint8_t PIC_ACK = 0x20;

void pic_acknowledge(uint8_t interrupt)
{
    if (interrupt >= 8)
    {
        outb(PIC2_COMMAND, PIC_ACK);
    }

    // PIC 1 always has to be acknowledged, even if PIC 2 raised the interrupt.
    outb(PIC1_COMMAND, PIC_ACK);
}

enum pic_initialize_command : uint8_t
{
    word_4 = 0x01,
    single = 0x02,
    internal_4 = 0x04,
    level = 0x08,
    init = 0x10,
};

enum pic_environment_command : uint8_t
{
    mode_8086 = 0x01,
    mode_auto = 0x02,
    buffered_slave = 0x08,
    buffered_master = 0x0C,
    special_fully_nested = 0x10,
};

enum pic1_interrupts : uint8_t
{
    pic1_none = 0x00,
    timer = 0x01,
    keyboard = 0x02,
    pic2 = 0x04,
    com2 = 0x08,
    com1 = 0x10,
    lpt2 = 0x20,
    floppy = 0x40,
    lpt1 = 0x80,
};

enum pic2_interrupts : uint8_t
{
    pic2_none = 0x00,
    clock = 0x01,
    general1 = 0x02,
    general2 = 0x04,
    general3 = 0x08,
    ps2_mouse = 0x10,
    coprocessor = 0x20,
    primary_disk = 0x40,
    secondary_disk = 0x80,
};

void pic_initialze()
{
    // Word 1 = Initialize
    enum pic_initialize_command word_1 = init | word_4;
    outb(PIC1_COMMAND, word_1);
    outb(PIC2_COMMAND, word_1);

    // Word 2 = Vector Offset
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    // Word 3 = Master/Slave Configuration
    outb(PIC1_DATA, 4); // Master PIC has a slave on IRQ2
    outb(PIC2_DATA, 2); // Slave is cascade

    // Word 4 = Environment Information
    enum pic_environment_command word_4 = mode_8086;
    outb(PIC1_DATA, word_4);
    outb(PIC2_DATA, word_4);

    // Word 5 = Bitmask of disabled interrupts (set = disabled, clear = enabled)
    enum pic1_interrupts pic1_enabled = pic2 | keyboard;
    enum pic2_interrupts pic2_enabled = pic2_none;
    outb(PIC1_DATA, ~pic1_enabled);
    outb(PIC2_DATA, ~pic2_enabled);
}
