#ifndef __GDT_H__
#define __GDT_H__

#define ACCESS_GDT_DESC   0b00000001
#define ACCESS_RW         0b00000010
#define ACCESS_EXEC_CODE  0b00001000
#define ACCESS_CODEORDATA 0b00010000
#define ACCESS_USER       0b01100000
#define ACCESS_INMEM      0b10000000

#define NUM_GDT_ENTRIES 6


#include "Kernel/Types.h"

typedef struct __attribute__((__packed__)) {
	u16 size;
	u32 offset;
} __attribute__((__packed__)) GDTReg;

typedef struct  {
	u16 limit;
	u16 baselo;
	u8	basemid;
	u8 flags;
	u8 grand;
	u8 basehi;
} __attribute__((__packed__)) GDTEntry;

typedef struct {
    u32 prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
  	u32 esp0;       // The stack pointer to load when we change to kernel mode.
    u32 ss0;        // The stack segment to load when we change to kernel mode.
   	u32 esp1;       // everything below here is unusued now.. 
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;         
	u32 cs;        
    u32 ss;        
    u32 ds;        
    u32 fs;       
    u32 gs;         
    u32 ldt;      
    u16 trap;
    u16 iomap_base;
} __attribute__((__packed__)) TSSEntry;

GDTEntry encodeGdtEntry(u32 maxAddr, u32 baseAddr, u8 flags, u8 grand);

void loadGdt(GDTEntry* toLoad, size_t size);

void gdt_init();
void writeTSS();
void setTSSStack(u32 stack);



#endif /* GDT_H_ */