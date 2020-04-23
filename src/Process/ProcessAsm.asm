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
    pop %eax ; Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop.
    or %eax, $0x200 ; Set the IF flag.
    push %eax ; Push the new EFLAGS value back onto the stack. 
    lea eax, [addToStack]
    push eax

    iretd

addToStack:
    add esp, 4