#include "idt.h"
#include "serial.h"

#define MAX_INTERRUPTS 1
#define CODE_SEGMENT 0x08

struct interrupt_descriptor
{
    unsigned long offset;
    unsigned short segment_selector;
    enum interrupt_gate_type gate_type;
    unsigned char privilege_level;
};

struct interrupt_descriptor_unsafe interrupts[MAX_INTERRUPTS];
struct interrupt_descriptor_table table = { .size = sizeof interrupts, .interrupts = interrupts };

void load_interrupt_descriptor(struct interrupt_descriptor const *descriptor, struct interrupt_descriptor_unsafe *target)
{
    target->offset_low = descriptor->offset & 0xFFFF;
    target->segment_selector = descriptor->segment_selector;
    target->gate_type = descriptor->gate_type;
    target->privilege_level = descriptor->privilege_level;
    target->present = true;
    target->offset_high = (descriptor->offset >> 16) & 0xFFFF;
}

struct interrupt_frame
{
    unsigned long eflags;
    unsigned long cs;
    unsigned long eip;
};

__attribute__((interrupt)) void interrupt_0_handler(__attribute__((unused)) struct interrupt_frame *frame)
{        
    serial_write("Someone tried to divide by zero.\n");
}

void initialze_interrupts()
{
    struct interrupt_descriptor interrupt_0 = {
        .offset = (unsigned long)&interrupt_0_handler,
        .segment_selector = CODE_SEGMENT,
        .gate_type = interrupt_32,
        .privilege_level = 0
    };
    load_interrupt_descriptor(&interrupt_0, interrupts);

    load_interrupt_descriptor_table(&table);
}