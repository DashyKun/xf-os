section .multiboot
    MAGIC    equ 0x1BADB002
    FLAGS    equ 0x0
    CHECKSUM equ -(MAGIC + FLAGS)

    align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
    align 16
stack_bottom:
    resb 16384 ; 16 kb stck
stack_top:

section .text
global _start
_start:
    mov esp, stack_top

    extern kmain
    call kmain

    cli
.hang:
    hlt
    jmp .hang