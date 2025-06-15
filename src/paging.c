#include <stdint.h>
#include <stddef.h>
#include "logger.h"
#include "instructions/paging.h"

struct page_table_entry
{
    bool is_present : 1;
    bool is_writable : 1;
    bool is_user : 1;
    bool is_write_through : 1;
    bool is_cache_disabled : 1;
    bool is_accessed : 1;
    bool is_dirty : 1;
    bool has_attribute_table : 1;
    bool is_global : 1;
    uint8_t : 3;
    uint32_t frame_address : 20;
} __attribute__((packed));

constexpr size_t page_size = 4096;
constexpr size_t page_table_size = 1024;
static struct page_directory_entry kernel_page_directory[page_table_size] __attribute__((aligned(4096)));
static struct page_table_entry identity_page_table_0[page_table_size] __attribute__((aligned(4096)));

void identity_map_block(uintptr_t start_of_block)
{
    constexpr int last_10_bits = 0x3FF;
    size_t table_index = (start_of_block >> 12) & last_10_bits;
    size_t directory_index = (start_of_block >> 22) & last_10_bits;

    // For now, we know that directory_index is always zero, so we can safely refer to page_table_0.
    identity_page_table_0[table_index].is_present = true;
    identity_page_table_0[table_index].is_writable = true;
    identity_page_table_0[table_index].is_user = false;
    identity_page_table_0[table_index].is_write_through = true;
    identity_page_table_0[table_index].is_cache_disabled = false;
    identity_page_table_0[table_index].is_accessed = false;
    identity_page_table_0[table_index].is_dirty = false;
    identity_page_table_0[table_index].has_attribute_table = false;
    identity_page_table_0[table_index].is_global = false;
    identity_page_table_0[table_index].frame_address = start_of_block >> 12;

    if (table_index == 0)
    {
        kernel_page_directory[directory_index].is_present = true;
        kernel_page_directory[directory_index].is_writable = true;
        kernel_page_directory[directory_index].is_user = false;
        kernel_page_directory[directory_index].is_write_through = true;
        kernel_page_directory[directory_index].is_cache_disabled = false;
        kernel_page_directory[directory_index].is_accessed = false;
        kernel_page_directory[directory_index].is_megabyte = false;
        kernel_page_directory[directory_index].table_address = (uintptr_t)(&identity_page_table_0) >> 12;
    }
}

void paging_initialize()
{
    constexpr int end_of_identity = 4 * 1024 * 1024; // 4MB arbitrarily chosen

    for (uintptr_t start_of_block = 0; start_of_block < end_of_identity; start_of_block += page_size)
    {
        logf(log_debug, "Identity mapping block %X.", start_of_block);
        identity_map_block(start_of_block);
    }

    logf(log_debug, "Loading page directory from %X.", kernel_page_directory);
    load_page_directory(kernel_page_directory);
    logf(log_info, "Paging is initialized.");
}