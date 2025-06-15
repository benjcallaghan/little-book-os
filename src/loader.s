global loader ; the entry symbol for ELF
KERNEL_STACK_SIZE equ 4096 ; size of stack in bytes
PAGE_SIZE equ 4096
END_OF_IDENTITY equ 4 * 1024 * 1024 ; identity map the first 4 MB
extern kmain ; the function kmain is defined elsewhere

section .note.GNU-stack noalloc noexec nowrite progbits ; disables execution from the stack

section .bss
align 4 ; align at 4 bytes
kernel_stack: ; label points to beginning of memory
    resb KERNEL_STACK_SIZE ; reserve stack for the kernel
align PAGE_SIZE ; align at 1 page
boot_page_directory:
    resb PAGE_SIZE
boot_page_table:
    resb PAGE_SIZE

section .text ; start of the text (code) section
align 4 ; the code must be 4 byte aligned
loader: ; the loader label (defined as entry point in linker script)
    ; Avoid using registers eax and ebx until the call to kmain, to avoid trampling the GRUB multiboot structures.

    ; Initialize all entries in boot_page_table with identity mapping
    mov edi, boot_page_table
    mov ecx, 0

start_page_table_loop:
    cmp ecx, END_OF_IDENTITY
    jge end_page_table_loop

    mov edx, ecx
    or edx, 0x0000000B ; set is_present, is_writable, and is_write_through
    mov [edi], edx

    add edi, 4
    add ecx, PAGE_SIZE
    jmp start_page_table_loop

end_page_table_loop:
    ; Add one entry to the page directory, pointing to boot_page_table
    mov edi, boot_page_directory
    mov edx, boot_page_table
    or edx, 0x0000000B ; set is_present, is_writable, and is_write_through
    mov [edi], edx
    
    ; Point the CPU to the page directory
    mov edx, boot_page_directory
    mov cr3, edx

    ; Enable paging and protection
    mov edx, cr0
    or edx, 0x80000001
    mov cr0, edx

    mov esp, kernel_stack + KERNEL_STACK_SIZE ; point esp to the start of the stack (end of memory area)

    push ebx ; Pointer to the multiboot structure
    push eax ; Bootloader magic number
    call kmain ; call the function, the result will be in eax
.loop:
    hlt
    jmp .loop ; loop forever
