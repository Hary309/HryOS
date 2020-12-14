global sys_call_1

; void sys_call_1(uint32_t id, uint32_t arg)
sys_call_1:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    int 50
    ret