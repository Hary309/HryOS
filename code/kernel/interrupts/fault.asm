%macro create_fault 1
global fault%1
fault%1:
    push %1
    jmp fault_common
%endmacro

create_fault 0
create_fault 1
create_fault 2
create_fault 3
create_fault 4
create_fault 5
create_fault 6
create_fault 7
create_fault 8
create_fault 9
create_fault 10
create_fault 11
create_fault 12
create_fault 13
create_fault 14
create_fault 16
create_fault 17
create_fault 18
create_fault 19
create_fault 20

extern fault_handler

fault_common:
    pushad
    mov ecx, esp ; ecx - first argument in fastcall 
    call fault_handler

    ; iret

