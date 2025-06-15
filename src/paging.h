#pragma once

void paging_initialize();
void *virtualize(void *physical);
void const *virtualize_const(void const *physical);