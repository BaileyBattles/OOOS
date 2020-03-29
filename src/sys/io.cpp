#include "kernel/types.h"
#include "sys/io.h"

void outb(u8 byte, u16 port){
    __asm__ __volatile__("out %%al, %%dx" : : "a" (byte), "d" (port));
}

u8 inb(u16 port){
    u8 result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}