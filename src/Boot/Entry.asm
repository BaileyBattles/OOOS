global loader
global stack
global stack_ptr

extern kernelMain

MODULEALIGN equ 0x1
MEMINFO     equ 0x2
VIDINFO     equ 0x4
FLAGS equ MODULEALIGN | MEMINFO | VIDINFO
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
  dd ZERO    
  dd ZERO   

section .text

STACKSIZE equ 0x5000

loader:
  mov esp, stack+STACKSIZE
  push eax ; argument 2 to main
  push ebx ; argument 1 to main

  call kernelMain

  cli

hang:
  hlt
  jmp hang

section .bss
align 4
stack:
  resb STACKSIZE
stack_ptr: