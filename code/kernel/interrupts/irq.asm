%macro create_irq 1
global irq%1
irq%1:
    push %1
    jmp irq_common
%endmacro

create_irq 0
create_irq 1
create_irq 2
create_irq 3
create_irq 4
create_irq 5
create_irq 6
create_irq 7
create_irq 8
create_irq 9
create_irq 10
create_irq 11
create_irq 12
create_irq 13
create_irq 14
create_irq 15

extern irq_handler

irq_common:
    pushad
    mov ecx, esp ; ecx - first argument in fastcall 
    call irq_handler
    popad

    add esp, 4 ; remove irq id from stack

    iret

global load_idt
global enable_interrupts

load_idt:
	mov eax, [esp + 4]
	lidt [eax]
	ret

enable_interrupts:
    sti
    ret
