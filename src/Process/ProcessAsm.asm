global get_eip
get_eip: 
    mov eax, [esp]
    ret

global enterUserMode
enterUserMode: 
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
	mov fs, ax
	mov gs, ax

    ;Setup the stack
    push 0x24
    push esp
    pushf
    lea eax, [addToStack]
    push eax

    iretd

addToStack:
    add esp, 4