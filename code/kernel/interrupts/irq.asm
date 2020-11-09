global irq0
irq0:
    push 0
    jmp irq_common
    
global irq1
irq1:
    push 1
    jmp irq_common
    
global irq2
irq2:
    push 2
    jmp irq_common
 
global irq3
irq3:
    push 3
    jmp irq_common
    
global irq4
irq4:
    push 4
    jmp irq_common
    
global irq5
irq5:
    push 5
    jmp irq_common
 
global irq6
irq6:
    push 6
    jmp irq_common
 
global irq7
irq7:
    push 7
    jmp irq_common
 
global irq8
irq8:
    push 8
    jmp irq_common
 
global irq9
irq9:
    push 9
    jmp irq_common
 
global irq10
irq10:
    push 10
    jmp irq_common
 
global irq11
irq11:
    push 11
    jmp irq_common
 
global irq12
irq12:
    push 12
    jmp irq_common
 
global irq13
irq13:
    push 13
    jmp irq_common
    
global irq14
irq14:
    push 14
    jmp irq_common

global irq15
irq15:
    push 15
    jmp irq_common

extern irq_handler

global irq_common
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
