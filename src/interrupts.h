#pragma once
#include <stdint.h>

struct interrupt_frame
{
    uint32_t eflags;
    uint32_t cs;
    uint32_t eip;
};

/**
 * Initializes interrupt handlers
 */
void interrupts_initialize();