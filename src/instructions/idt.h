#pragma once
#include <stdint.h>

enum interrupt_gate_type : uint8_t
{
    task_gate = 0x5,
    interrupt_16 = 0x6,
    trap_16 = 0x7,
    interrupt_32 = 0xE,
    trap_32 = 0xF,
};

struct interrupt_descriptor_unsafe
{
    uint16_t offset_low : 16;
    uint16_t segment_selector : 16;
    uint8_t : 8;
    enum interrupt_gate_type gate_type : 4;
    bool : 1;
    uint16_t privilege_level : 2;
    bool present : 1;
    uint16_t offset_high : 16;
} __attribute__((packed));

struct interrupt_descriptor_table
{
    uint16_t size; // Size in bytes, not number of interrupts
    struct interrupt_descriptor_unsafe *interrupts;
} __attribute((packed));

/**
 * Loads the Interrupt Descriptor Table (IDT).
 */
void load_interrupt_descriptor_table(struct interrupt_descriptor_table const *table);