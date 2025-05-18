#pragma once

enum interrupt_gate_type
{
    task_gate = 0x5,
    interrupt_16 = 0x6,
    trap_16 = 0x7,
    interrupt_32 = 0xE,
    trap_32 = 0xF,
};

struct interrupt_descriptor_unsafe
{
    unsigned short offset_low : 16;
    unsigned short segment_selector : 16;
    unsigned char : 8;
    enum interrupt_gate_type gate_type : 4;
    unsigned char : 1;
    unsigned char privilege_level : 2;
    bool present : 1;
    unsigned short offset_high : 16;
} __attribute__((packed));

struct interrupt_descriptor_table
{
    unsigned short size; // Size in bytes, not number of interrupts
    struct interrupt_descriptor_unsafe *interrupts;
} __attribute((packed));

/**
 * Loads the Interrupt Descriptor Table (IDT).
 */
void load_interrupt_descriptor_table(struct interrupt_descriptor_table const *table);