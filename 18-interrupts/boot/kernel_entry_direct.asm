[bits 32]
[global _start]

extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Call kernel main
    call kernel_main
    
    ; Infinite loop if kernel returns
    cli
    hlt
    jmp $

section .bss
align 4
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top: 