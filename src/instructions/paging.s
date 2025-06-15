global load_page_directory

section .note.GNU-stack noalloc noexec nowrite progbits ; disables execution from the stack

section .text

; load_page_directory: loads the given page directory into the cr3 register
; stack: [esp + 4] pointer to the page directory
;        [esp] return address
load_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax

    ; Enable paging and protection
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    
    ret
