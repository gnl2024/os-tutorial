[global switch_to_process]
[extern current_process]

; Switch to a new process
; void switch_to_process(process_t *proc)
switch_to_process:
    ; Save current process state
    push ebp
    mov ebp, esp
    
    ; Get process pointer from parameter
    mov eax, [ebp + 8]  ; process_t pointer
    
    ; Save current registers
    push eax  ; Save process pointer
    
    ; Load new process state
    mov esp, [eax + 20]  ; regs.esp
    mov ebp, [eax + 24]  ; regs.ebp
    
    ; Update current_process pointer
    mov [current_process], eax
    
    ; Set segment registers
    mov ax, [eax + 44]  ; regs.cs
    mov cs, ax
    mov ax, [eax + 48]  ; regs.ds
    mov ds, ax
    mov ax, [eax + 52]  ; regs.es
    mov es, ax
    mov ax, [eax + 56]  ; regs.fs
    mov fs, ax
    mov ax, [eax + 60]  ; regs.gs
    mov gs, ax
    mov ax, [eax + 64]  ; regs.ss
    mov ss, ax
    
    ; Set general registers
    mov eax, [eax + 4]   ; regs.eax
    mov ebx, [eax + 8]   ; regs.ebx
    mov ecx, [eax + 12]  ; regs.ecx
    mov edx, [eax + 16]  ; regs.edx
    mov esi, [eax + 28]  ; regs.esi
    mov edi, [eax + 32]  ; regs.edi
    
    ; Set flags
    mov eax, [eax + 40]  ; regs.eflags
    push eax
    popfd
    
    ; Jump to new process
    mov eax, [eax + 36]  ; regs.eip
    jmp eax

; Save current process state
; void save_process_state(process_t *proc)
[global save_process_state]
save_process_state:
    push ebp
    mov ebp, esp
    
    mov eax, [ebp + 8]  ; process_t pointer
    
    ; Save general registers
    mov [eax + 4], eax   ; regs.eax
    mov [eax + 8], ebx   ; regs.ebx
    mov [eax + 12], ecx  ; regs.ecx
    mov [eax + 16], edx  ; regs.edx
    mov [eax + 28], esi  ; regs.esi
    mov [eax + 32], edi  ; regs.edi
    mov [eax + 20], esp  ; regs.esp
    mov [eax + 24], ebp  ; regs.ebp
    
    ; Save segment registers
    mov bx, cs
    mov [eax + 44], bx   ; regs.cs
    mov bx, ds
    mov [eax + 48], bx   ; regs.ds
    mov bx, es
    mov [eax + 52], bx   ; regs.es
    mov bx, fs
    mov [eax + 56], bx   ; regs.fs
    mov bx, gs
    mov [eax + 60], bx   ; regs.gs
    mov bx, ss
    mov [eax + 64], bx   ; regs.ss
    
    ; Save flags
    pushfd
    pop eax
    mov [eax + 40], eax  ; regs.eflags
    
    ; Save instruction pointer
    mov eax, [esp]
    mov [eax + 36], eax  ; regs.eip
    
    pop ebp
    ret 