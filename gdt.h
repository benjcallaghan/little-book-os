#pragma once

struct segment_descriptor_table_descriptor
{
    unsigned short size;
    unsigned long address;
} __attribute__((packed));

/**
 * Loads the Global Descriptor Table (GDT) and updates all segment registers.
 */
void load_global_descriptor_table(struct segment_descriptor_table_descriptor *descriptor);