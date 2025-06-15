#pragma once

struct page_directory_entry
{
    bool is_present : 1;
    bool is_writable : 1;
    bool is_user : 1;
    bool is_write_through : 1;
    bool is_cache_disabled : 1;
    bool is_accessed : 1;
    bool : 1;
    bool is_megabyte : 1;
    uint8_t : 4;
    uint32_t table_address : 20;
} __attribute__((packed));

void load_page_directory(struct page_directory_entry *directory);