struc process
    .pad   resb 24
    .stack_top resd 1
endstruc

struc stack
    .bottom   resd 1
    .top      resd 1
    .size     resd 1
endstruc

global switch_to_task

extern current_process

; C declaration:
;   void switch_to_task(process* next);

switch_to_task:
    ; Push all registers to stack
    push eax
    push ecx
    push edx
    push ebx
    push ebp
    push esi
    push edi

    ; Check if current_process is null  
    mov esi, [current_process]
    and esi, esi
    jz load_task

    ; if no, save stack pointer
    mov esi, [current_process]
    mov [esi + process.stack_top], esp
 
 load_task:
    ; Load new stack pointer
    mov esi, [esp + 4 * 7 + 4] ; 4 * all registers + return addr
    mov esp, [esi + process.stack_top]

    ; Update current process pointer
    mov [current_process], esi

    ; Restore all registers
    ; Without esp, because it is already correct
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax

    ret
