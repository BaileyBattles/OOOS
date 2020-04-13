#include "sys/io.h"
#include "kernel/types.h"

u8 inb(u16 port)
{
	u8 ret;
	asm volatile("inb %%dx, %%al":"=a"(ret):"d"(port));
	return ret;
}
u16 inw(u16 port)
{
	u16 ret;
	asm volatile("inw %%dx, %%ax":"=a"(ret):"d"(port));
	return ret;
}
u32 inl(u16 port)
{
	u32 ret;
	asm volatile("inl %%dx, %%eax":"=a"(ret):"d"(port));
	return ret;
}
void outb(u16 port, u8 value)
{
	asm volatile("outb %%al, %%dx": :"d" (port), "a" (value));
}
void outw(u16 port, u16 value)
{
	asm volatile("outw %%ax, %%dx": :"d" (port), "a" (value));
}
void outl(u16 port, u32 value)
{
	asm volatile("outl %%eax, %%dx": :"d" (port), "a" (value));
}