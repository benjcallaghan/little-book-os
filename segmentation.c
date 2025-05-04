#include "gdt.h"
#define SEGMENTS 3

struct segment_descriptor
{
    unsigned long base;
    unsigned long limit;
    unsigned char access;
    unsigned char flags;
};

struct segment_descriptor_table
{
    unsigned long long values[SEGMENTS];
};

const struct segment_descriptor_table gdt;

void load_descriptor(struct segment_descriptor *descriptor, unsigned char target[])
{
    // Encode the limit
    target[0] = descriptor->limit & 0xFF;
    target[1] = (descriptor->limit >> 8) & 0xFF;
    target[6] = (descriptor->limit >> 16) & 0x0F;

    // Encode the base
    target[2] = descriptor->base & 0xFF;
    target[3] = (descriptor->base >> 8) & 0xFF;
    target[4] = (descriptor->base >> 16) & 0xFF;
    target[7] = (descriptor->base >> 24) & 0xFF;

    // Encode the access byte
    target[5] = descriptor->access;

    // Encode the flags
    target[6] |= (descriptor->flags << 4);
}

void initialze_segmentation()
{
    struct segment_descriptor null = {
        .base = 0,
        .limit = 0,
        .access = 0,
        .flags = 0,
    };
    load_descriptor(&null, (unsigned char *)(gdt.values));

    struct segment_descriptor code = {
        .base = 0,
        .limit = 0xFFFFF,
        .access = 0x9A, // Present, Privilege 0, Code/Data, Executable, Readable
        .flags = 0xC,   // Block Limit, 32-bit
    };
    load_descriptor(&code, (unsigned char *)(gdt.values + 1));

    struct segment_descriptor data = {
        .base = 0,
        .limit = 0xFFFFF,
        .access = 0x92, // Present, Privilege 0, Code/Data, Writable
        .flags = 0xC,   // Block Limit, 32-bit
    };
    load_descriptor(&data, (unsigned char *)(gdt.values + 2));

    struct segment_descriptor_table_descriptor descriptor = {
        .address = (unsigned long)&gdt,
        .size = sizeof(gdt)};
    load_global_descriptor_table(&descriptor);
}