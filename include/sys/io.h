#ifndef IO_H
#define IO_H

#include "kernel/types.h"

unsigned char inb(u16 port);
void outb(u16 port, u8 data);
unsigned short inw(u16 port);
void outw(u16 port, u16 data);

#endif
