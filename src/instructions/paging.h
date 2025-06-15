#pragma once

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

void load_page_directory(struct page_directory_entry *directory);