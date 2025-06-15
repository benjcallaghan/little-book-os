ENTRY(loader) /* the name of the entry label */

HIDDEN(page_size = 0x1000); /* 4 KB */
HIDDEN(virtual_higher_half = 0xC0000000); /* 3 GB */
HIDDEN(hardware_end = 0x100000); /* 1 MB */

SECTIONS {
  . = virtual_higher_half + hardware_end; /* Move to the higher half, but leave room for BIOS and GRUB. */

  .multiboot ALIGN (page_size) : AT(ADDR(.multiboot) - virtual_higher_half)
  {
    *(.multiboot)
  }

  .text ALIGN (page_size) : AT(ADDR(.text) - virtual_higher_half)
  {
    *(.text) /* all text sections from all files */
  }

  .rodata ALIGN (page_size): AT(ADDR(.rodata) - virtual_higher_half)
  {
    *(.rodata) /* all read-only data sections from all files */
  }

  .data ALIGN (page_size): AT(ADDR(.data) - virtual_higher_half)
  {
    *(.data) /* all data sections from all files */
  }

  .bss ALIGN (page_size): AT(ADDR(.bss) - virtual_higher_half)
  {
    *(COMMON) /* all COMMON sections from all files */
    *(.bss) /* all bss sections from all files */
  }
}