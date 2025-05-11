#include "gdt.h"

struct segment_descriptor
{
    unsigned long base;
    unsigned long limit;
    unsigned char access;
    unsigned char flags;
};

struct segment_descriptor_table gdt;

void load_descriptor(struct segment_descriptor const *descriptor, struct segment_descriptor_unsafe *target)
{
    target->limit_low = (descriptor->limit) & 0xFFFF;
    target->limit_high = (descriptor->limit >> 16) & 0xFF;

    target->base_low = (descriptor->base) & 0xFFFFFF;
    target->base_high = (descriptor->base >> 24) & 0xFF;

    target->access = descriptor->access;
    target->flags = descriptor->flags;
}

void initialze_segmentation()
{
    struct segment_descriptor null = {
        .base = 0,
        .limit = 0,
        .access = 0,
        .flags = 0,
    };
    load_descriptor(&null, gdt.values);

    struct segment_descriptor code = {
        .base = 0,
        .limit = 0xFFFFF,
        .access = 0x9A, // Present, Privilege 0, Code/Data, Executable, Readable
        .flags = 0xC,   // Block Limit, 32-bit
    };
    load_descriptor(&code, gdt.values + 1);

    struct segment_descriptor data = {
        .base = 0,
        .limit = 0xFFFFF,
        .access = 0x92, // Present, Privilege 0, Code/Data, Writable
        .flags = 0xC,   // Block Limit, 32-bit
    };
    load_descriptor(&data, gdt.values + 2);

    struct segment_descriptor_table_descriptor descriptor = {
        .address = &gdt,
        .size = sizeof(gdt)
    };
    load_global_descriptor_table(&descriptor);
}