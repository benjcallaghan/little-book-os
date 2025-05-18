#include "gdt.h"
#include <stddef.h>

#define NUM_SEGMENTS 3
#define MAX_SEGMENT_LIMIT 0xFFFFF // largest 20-bit value

struct segment_descriptor
{
    uintptr_t base;
    size_t limit;
    enum segment_access access;
    enum segment_flags flags;
};

struct segment_descriptor_unsafe global_segments[NUM_SEGMENTS];
struct segment_descriptor_table global_table = { .size = sizeof global_segments, .segments = global_segments };

void load_segment_descriptor(struct segment_descriptor const *descriptor, struct segment_descriptor_unsafe *target)
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
    load_segment_descriptor(&null, global_table.segments);

    struct segment_descriptor code = {
        .base = 0,
        .limit = MAX_SEGMENT_LIMIT,
        .access = present | code_data | executable | readable_or_writable,
        .flags = page_granularity | size_32,
    };
    load_segment_descriptor(&code, global_table.segments + 1);

    struct segment_descriptor data = {
        .base = 0,
        .limit = MAX_SEGMENT_LIMIT,
        .access = present | code_data | readable_or_writable,
        .flags = page_granularity | size_32,
    };
    load_segment_descriptor(&data, global_table.segments + 2);

    load_global_descriptor_table(&global_table);
}