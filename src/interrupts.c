#include "idt.h"
#include "serial.h"
#include "printf.h"
#include "fb.h"
#include <stddef.h>

#define MAX_INTERRUPTS 14
#define CODE_SEGMENT 0x08

struct interrupt_frame
{
    uint32_t eflags;
    uint32_t cs;
    uint32_t eip;
};

struct interrupt_descriptor
{
    union
    {
        void (*no_error_code)(struct interrupt_frame const *);
        void (*with_error_code)(struct interrupt_frame const *, uint32_t);
    } handler;

    int segment_selector;
    enum interrupt_gate_type gate_type;
    int privilege_level;
};

struct interrupt_descriptor_unsafe interrupts[MAX_INTERRUPTS];
struct interrupt_descriptor_table table = {.size = sizeof interrupts, .interrupts = interrupts};

void load_interrupt_descriptor(struct interrupt_descriptor const *descriptor, struct interrupt_descriptor_unsafe *target)
{
    // Whether the interrupt has an error code is irrelevant at this stage. In both cases, the pointer width is the same.
    target->offset_low = (uintptr_t)descriptor->handler.no_error_code & 0xFFFF;
    target->segment_selector = descriptor->segment_selector;
    target->gate_type = descriptor->gate_type;
    target->privilege_level = descriptor->privilege_level;
    target->present = true;
    target->offset_high = ((uintptr_t)descriptor->handler.no_error_code >> 16) & 0xFFFF;
}

__attribute__((interrupt,target("general-regs-only"))) void div_0_handler(struct interrupt_frame const *frame)
{
    fb_clear();
    printf(fb_write_char, "#DE EFLAGS=%X,CS=%X,EIP=%X", frame->eflags, frame->cs, frame->eip);
    printf(serial_write_char, "Someone tried to divide by zero. EFLAGS=%X,CS=%X,EIP=%X\n", frame->eflags, frame->cs, frame->eip);
}

__attribute__((interrupt,target("general-regs-only"))) void general_protection_fault_handler(struct interrupt_frame const *frame, uint32_t error_code)
{
    fb_clear();
    printf(fb_write_char, "#GP CODE=%X,EFLAGS=%X,CS=%X,EIP=%X", error_code, frame->eflags, frame->cs, frame->eip);
    printf(serial_write_char, "ERROR: General Protection Fault. Error Code=%X,EFLAGS=%X,CS=%X,EIP=%X\n", error_code, frame->eflags, frame->cs, frame->eip);
}

void initialze_interrupts()
{
    struct interrupt_descriptor interrupt_0 = {
        .handler = {div_0_handler},
        .segment_selector = CODE_SEGMENT,
        .gate_type = trap_32,
        .privilege_level = 0,
    };
    load_interrupt_descriptor(&interrupt_0, interrupts);

    struct interrupt_descriptor interrupt_13 = {
        .handler = {.with_error_code = general_protection_fault_handler},
        .segment_selector = CODE_SEGMENT,
        .gate_type = trap_32,
        .privilege_level = 0,
    };
    load_interrupt_descriptor(&interrupt_13, interrupts + 13);

    load_interrupt_descriptor_table(&table);
}