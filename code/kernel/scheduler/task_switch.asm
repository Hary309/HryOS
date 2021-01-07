struc process
    .pad   resb 20
    .esp   resd 1
endstruc

struc args
    .current resd 1
    .next resd 1
endstruc

;C declaration:
;   void switch_to_task(registers* curr, registers* next);

global switch_to_task

switch_to_task:
 
    ;Save previous task's state
 
    ;Notes:
    ;  For cdecl; EAX, ECX, and EDX are already saved by the caller and don't need to be saved again
    ;  EIP is already saved on the stack by the caller's "CALL" instruction
    ;  The task isn't able` to change CR3 so it doesn't need to be saved
    ;  Segment registers are constants (while running kernel code) so they don't need to be saved
 
    push ebx
    push esi
    push edi
    push ebp

    ; save current state

    mov edi,[esp + args.current]
    mov [edi + process.esp],esp
 
    ;Load next task's state
 
    mov esi, [esp + args.next]
    mov esp, [esi + process.esp]
 
    pop ebp
    pop edi
    pop esi
    pop ebx
 
    ret   

