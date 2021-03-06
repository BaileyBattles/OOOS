global get_eip
get_eip: 
    mov eax, [esp]
    ret

global read_eip
read_eip:
  pop eax
  jmp eax

GLOBAL enteruser ;you may need to remove this _ to work right.. 
EXTERN kernelLoop
enteruser:
    mov ebx, [esp + 4]
    mov ax,0x23
    mov ds,ax
    mov es,ax 
    mov fs,ax 
    mov gs,ax ;we don't need to worry about SS. it's handled by iret
 
    mov eax,esp
    push 0x23 ;user data segment with bottom 2 bits set for ring 3
    push eax ;push our current ss for the iret stack frame
    pushf
    pop eax ; Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop.
    or eax, 0x200 ; Set the IF flag.
    push eax ; Push the new EFLAGS value back onto the stack. 
    push 0x1B; ;user code segment with bottom 2 bits set for ring 3
    push ebx ;may need to remove the _ for this to work right 
    iret

addToStack:
    add esp, 4

global jumpToCode
jumpToCode:
    cli
    mov edx, [esp + 4]
    mov esp, [esp + 8]
    mov ebp, ecx
    mov eax, 0x10987
    sti
    jmp edx