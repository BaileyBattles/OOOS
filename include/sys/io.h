#ifndef __IO_H__
#define __IO_H__
#include "kernel/types.h"

void outb(u8 byte, u16 port);
u8 inb(u16 port);
#endif