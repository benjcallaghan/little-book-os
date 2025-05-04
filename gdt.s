global load_global_descriptor_table

CODE_SEGMENT equ 0x08
DATA_SEGMENT equ 0x10

section .note.GNU-stack noalloc noexec nowrite progbits ; disables execution from the stack

section .text

; load_global_descriptor_table:
; stack: [esp + 4] pointer to table descriptor
;        [esp] return address
load_global_descriptor_table:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, DATA_SEGMENT
    mov ds, ax
    mov ss, ax
    mov es, ax

    jmp CODE_SEGMENT:.load_cs ; far jump to overwrite cs register
.load_cs:
    ret
