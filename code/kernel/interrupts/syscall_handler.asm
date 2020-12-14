extern syscall_handler

global syscall

syscall:
    push 0 ; push fake irq id
    pushad
    mov ecx, esp ; ecx - first argument in fastcall 
    call syscall_handler
    popad

    iret
