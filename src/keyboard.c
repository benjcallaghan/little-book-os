#include "interrupts.h"
#include "serial.h"
#include "pic.h"

constexpr uint8_t keyboard_pic_interrupt = 1;

__attribute__((interrupt, target("general-regs-only"))) void keyboard_interrupt_handler(__attribute__((unused)) struct interrupt_frame const *frame)
{
    serial_write("A key was pressed\n");
    pic_acknowledge(keyboard_pic_interrupt);
}