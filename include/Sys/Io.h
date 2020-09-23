#ifndef IO_H
#define IO_H

#include "Kernel/Types.h"

u8 inb(u16 port);
void outb(u16 port, u8 data);
u16 inw(u16 port);
void outw(u16 port, u16 data);
u32 inl(u16 port);
void outl(u16 port, u32 data);

#endif
