%macro create_isr 1
global isr%1
isr%1:
    push %1
    jmp isr_common
%endmacro

create_isr 0
create_isr 1
create_isr 2
create_isr 3
create_isr 4
create_isr 5
create_isr 6
create_isr 7
create_isr 8
create_isr 9
create_isr 10
create_isr 11
create_isr 12
create_isr 13
create_isr 14
create_isr 15

extern pic_handler

isr_common:
    pushad
    mov ecx, esp ; ecx - first argument in fastcall 
    call pic_handler
    popad

    add esp, 4 ; remove isr id from stack

    iret

global load_idt

load_idt:
	mov eax, [esp + 4]
	lidt [eax]
	ret
