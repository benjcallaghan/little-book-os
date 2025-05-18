global load_interrupt_descriptor_table

section .note.GNU-stack noalloc noexec nowrite progbits ; disables execution from the stack

section .text

; load_interrupt_descriptor_table:
; stack: [esp + 4] pointer to table descriptor
;        [esp] return address
load_interrupt_descriptor_table:
    mov eax, [esp + 4]
    lidt [eax]
    ret
