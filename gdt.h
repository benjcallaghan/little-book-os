#pragma once

#define SEGMENTS 3

struct segment_descriptor_unsafe
{
    unsigned short limit_low: 16;
    unsigned long base_low: 24;
    unsigned char access: 8; // TODO: Split into pieces
    unsigned char limit_high: 4;
    unsigned char flags: 4; // TODO: Split into pieces
    unsigned char base_high: 8;
} __attribute__((packed));

struct segment_descriptor_table
{
    struct segment_descriptor_unsafe values[SEGMENTS];
};

struct segment_descriptor_table_descriptor
{
    unsigned short size;
    struct segment_descriptor_table const *address;
} __attribute__((packed));

/**
 * Loads the Global Descriptor Table (GDT) and updates all segment registers.
 */
void load_global_descriptor_table(struct segment_descriptor_table_descriptor const *descriptor);