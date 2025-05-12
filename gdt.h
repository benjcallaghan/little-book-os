#pragma once

#define SEGMENTS 3

enum segment_flags
{
    // When set, the segment's limit field is measured in 4 KiB blocks (i.e. pages). When clear, the limit field is measured in bytes.
    page_granularity = 0x8,

    // When set, the segment is 32-bit. When clear, the segment is 16-bit.
    size_32 = 0x4,

    // When set, the segment is 64-bit. This flag should not be used in conjuction with size_32.
    long_mode = 0x2,
};

struct segment_descriptor_unsafe
{
    unsigned short limit_low : 16;
    unsigned long base_low : 24;
    unsigned char access : 8; // TODO: Split into pieces
    unsigned char limit_high : 4;
    enum segment_flags flags : 4;
    unsigned char base_high : 8;
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