[extern isr_handler]
[extern irq_handler]

; Common ISR code
isr_common_stub:
    ; 1. Save CPU state
    pusha ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    mov ax, ds ; Lower 16-bits of eax = ds.
    push eax ; save the data segment descriptor
    mov ax, 0x10  ; kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 2. Call C handler
    call isr_handler
    
    ; 3. Restore state
    pop eax 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8 ; Cleans up the pushed error code and pushed ISR number
    sti
    iret ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

; Common IRQ code
irq_common_stub:
    pusha 
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq_handler ; Call C IRQ handler
    pop ebx  
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    sti
    iret 

; Generate ISR stubs programmatically
%macro isr_stub 1
isr%1:
    cli
    %if %1 == 8 || %1 == 10 || %1 == 11 || %1 == 12 || %1 == 13 || %1 == 14
        push byte %1
    %else
        push byte 0
        push byte %1
    %endif
    jmp isr_common_stub
%endmacro

; Generate IRQ stubs programmatically
%macro irq_stub 2
irq%1:
    cli
    push byte %1
    push byte %2
    jmp irq_common_stub
%endmacro

; Generate all ISR stubs
%assign i 0
%rep 32
    isr_stub i
    %assign i i+1
%endrep

; Generate all IRQ stubs
%assign i 0
%rep 16
    irq_stub i, i+32
    %assign i i+1
%endrep

; Export all ISR symbols
%assign i 0
%rep 32
    global isr%+i
    %assign i i+1
%endrep

; Export all IRQ symbols
%assign i 0
%rep 16
    global irq%+i
    %assign i i+1
%endrep 