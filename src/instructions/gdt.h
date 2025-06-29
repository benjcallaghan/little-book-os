#pragma once
#include <stdint.h>

enum segment_access : uint8_t
{
    // Must be set for any valid segment
    present = 0x80,

    // When set, the segment operates in privilege level 3 (lowest privilege).
    // When clear, the segment operates in privilege level 0 (highest privilege).
    low_privilege = 0x60,

    // When set, the segment is a code or data segment.
    // When clear, the segment is a system (task state) segment.
    code_data = 0x10,

    // When set, the segment is a code segment.
    // When clear, the segment is a data segment.
    executable = 0x08,

    // When set, the segment grows down (if a data segment) or is conforming to privilege levels (if a code segment).
    // When clear, the segment grows up (if a data segment) or is non-conforming (if a code segment).
    grows_down_or_conforming = 0x04,

    // When set, the segment is readable (if a code segment) or is writable (if a data segment)
    // Data segments are always readable, even if this flag is clear.
    // Code segments are never writable, even if this flag is set.
    readable_or_writable = 0x02,

    // When set, the segment has been accessed by the CPU. This flag should generally be set.
    accessed = 0x01,
};

enum segment_flags : uint8_t
{
    // When set, the segment's limit field is measured in 4 KiB blocks (i.e. pages).
    // When clear, the limit field is measured in bytes.
    page_granularity = 0x8,

    // When set, the segment is 32-bit.
    // When clear, the segment is 16-bit.
    size_32 = 0x4,

    // When set, the segment is 64-bit. This flag should not be used in conjuction with size_32.
    long_mode = 0x2,
};

struct segment_descriptor_unsafe
{
    uint16_t limit_low : 16;
    uint32_t base_low : 24;
    enum segment_access access : 8;
    uint8_t limit_high : 4;
    enum segment_flags flags : 4;
    uint8_t base_high : 8;
} __attribute__((packed));

struct segment_descriptor_table
{
    uint16_t size; // Size in bytes, not number of segments.
    struct segment_descriptor_unsafe *segments;
} __attribute__((packed));

/**
 * Loads the Global Descriptor Table (GDT) and updates all segment registers.
 */
void load_global_descriptor_table(struct segment_descriptor_table const *table);