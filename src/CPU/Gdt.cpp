#include "CPU/Gdt.h"

static GDTReg gdtr;
static GDTEntry gdt[NUM_GDT_ENTRIES];

GDTEntry makeEntry(u32 size, u32 baseAddr, u8 flags, u8 grand) {
	GDTEntry toReturn;
	toReturn.limit = size & 0xFFFF;
	toReturn.baselo = baseAddr & 0xFFFF;
	toReturn.basemid |= ((baseAddr & 0xFF0000) >> 16);
	toReturn.flags = flags;
	toReturn.grand = (size & 0xF0000) >> 16;
	toReturn.grand |= (grand << 4) & 0xF0;
	toReturn.basehi = ((baseAddr & 0xFF000000) >> 24);
	return toReturn;
}

void loadGdt() {
	size_t sizeOfGdt = NUM_GDT_ENTRIES * sizeof(GDTEntry) - 1;
	gdtr.size = sizeOfGdt;
	gdtr.offset = (u32) gdt;
	asm ("LGDT %[gdt]" : : [gdt] "m" (gdtr));
}

void gdt_init(){
    u8 kernelCode = ACCESS_RW | ACCESS_CODE | ACCESS_INMEM | ACCESS_CODEORDATA;
    u8 kernelData = ACCESS_RW | ACCESS_INMEM | ACCESS_CODEORDATA;
    u8 userCode = ACCESS_USER | ACCESS_RW | ACCESS_CODE | ACCESS_INMEM| ACCESS_CODEORDATA ;
    u8 userData = ACCESS_USER | ACCESS_RW | ACCESS_INMEM | ACCESS_CODEORDATA;
	gdt[0] = makeEntry(0, 0, 0, 0);
	gdt[1] = makeEntry(0xffffffff, 0, kernelCode, 0xCF);
	gdt[2] = makeEntry(0xffffffff, 0, kernelData, 0xCF);
    gdt[3] = makeEntry(0xffffffff, 0, userCode, 0xCF);
    gdt[4] = makeEntry(0xffffffff, 0, userData, 0xCF);

    
    loadGdt();
}