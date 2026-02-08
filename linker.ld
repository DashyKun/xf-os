ENTRY(_start)

SECTIONS {
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.text)
        *(.text.*)
    }
    
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }
    
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }
    
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
    
    .heap BLOCK(4K) : ALIGN(4K) {
        __heap_start = .;
        . = . + 1M;  /* 1MB heap */
        __heap_end = .;
    }
}