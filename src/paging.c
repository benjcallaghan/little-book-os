#include <stdint.h>
#include "logger.h"

struct page_directory_entry
{
    bool is_present : true;
    bool is_writable : true;
    bool is_user : true;
    bool is_write_through : true;
    bool is_cache_disabled : true;
    bool is_accessed : true;
    bool : true;
    bool is_megabyte : true;
    uint8_t : 4;
    uint32_t table_address : 20;
} __attribute__((packed));

struct page_table_entry
{
    bool is_present : true;
    bool is_writable : true;
    bool is_user : true;
    bool is_write_through : true;
    bool is_cache_disabled : true;
    bool is_accessed : true;
    bool is_dirty : true;
    bool has_attribute_table : true;
    bool is_global : true;
    uint8_t : 3;
    uint32_t frame_address : 20;
} __attribute__((packed));

constexpr int page_size = 4096;
static struct page_directory_entry kernel_page_directory[page_size] __attribute__((aligned(4096)));
static struct page_table_entry identity_page_table_0[page_size] __attribute__((aligned(4096)));

void paging_initialize()
{
    identity_page_table_0[0].is_present = true;
    identity_page_table_0[0].is_writable = true;
    identity_page_table_0[0].is_user = false;
    identity_page_table_0[0].is_write_through = true;
    identity_page_table_0[0].is_cache_disabled = false;
    identity_page_table_0[0].is_accessed = false;
    identity_page_table_0[0].is_dirty = false;
    identity_page_table_0[0].has_attribute_table = false;
    identity_page_table_0[0].is_global = false;
    identity_page_table_0[0].frame_address = 0;

    kernel_page_directory[0].is_present = true;
    kernel_page_directory[0].is_writable = true;
    kernel_page_directory[0].is_user = false;
    kernel_page_directory[0].is_write_through = true;
    kernel_page_directory[0].is_cache_disabled = false;
    kernel_page_directory[0].is_accessed = false;
    kernel_page_directory[0].is_megabyte = false;
    kernel_page_directory[0].table_address = (uintptr_t)(&identity_page_table_0) >> 12;

    logf(log_info, "Paging is initialized.");
}