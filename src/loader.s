global loader ; the entry symbol for ELF
KERNEL_STACK_SIZE equ 4096 ; size of stack in bytes
extern kmain ; the function kmain is defined elsewhere

section .note.GNU-stack noalloc noexec nowrite progbits ; disables execution from the stack

section .bss
align 4 ; align at 4 bytes
kernel_stack: ; label points to beginning of memory
    resb KERNEL_STACK_SIZE ; reserve stack for the kernel

section .text ; start of the text (code) section
align 4 ; the code must be 4 byte aligned
loader: ; the loader label (defined as entry point in linker script)
    mov esp, kernel_stack + KERNEL_STACK_SIZE ; point esp to the start of the stack (end of memory area)

    push ebx ; Pointer to the multiboot structure
    push eax ; Bootloader magic number
    call kmain ; call the function, the result will be in eax
.loop:
    hlt
    jmp .loop ; loop forever
