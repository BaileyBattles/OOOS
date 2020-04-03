global loader
global stack
global stack_ptr

extern main

MODULEALIGN equ 1<<0
MEMINFO equ 1<<1
FLAGS equ MODULEALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)
ZERO equ 0x00000000

section .mbheader
align 4
MultiBootHeader:
  dd MAGIC
  dd FLAGS
  dd CHECKSUM

  dd ZERO   
  dd ZERO   
  dd ZERO 
BSS_END_ADR:   
  dd ZERO    
  dd ZERO   

section .text

STACKSIZE equ 0x4000

loader:
  mov esp, stack+STACKSIZE
  push ebx ; argument 1 to main
  push eax ; argument 2 to main

  call main

  cli

hang:
  hlt
  jmp hang

section .bss
align 4
stack:
  resb STACKSIZE
stack_ptr: