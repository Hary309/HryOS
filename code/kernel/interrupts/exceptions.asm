%macro create_exception 1
global exception%1
exception%1:
    push %1
    jmp exception_common
%endmacro

create_exception 0
create_exception 1
create_exception 2
create_exception 3
create_exception 4
create_exception 5
create_exception 6
create_exception 7
create_exception 8
create_exception 9
create_exception 10
create_exception 11
create_exception 12
create_exception 13
create_exception 14
create_exception 16
create_exception 17
create_exception 18
create_exception 19
create_exception 20

extern exception_handler

exception_common:
    pushad
    mov ecx, esp ; ecx - first argument in fastcall 
    call exception_handler

    ; iret

