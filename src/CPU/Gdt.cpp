#include "CPU/Gdt.h"
#include "Util/Memcpy.h"

static GDTReg gdtr;
static GDTEntry gdt[NUM_GDT_ENTRIES];
static TSSEntry tss;

extern "C" void tss_flush();

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
	size_t sizeOfGdt = 6 * sizeof(GDTEntry) - 1;
	gdtr.size = sizeOfGdt;
	gdtr.offset = (u32) gdt;
	asm ("LGDT %[gdt]" : : [gdt] "m" (gdtr));
}

void writeTSS() {
	memory_set(&tss, '\0', sizeof(TSSEntry));
	tss.ss0 = 0x10;
	tss.esp0 = 0x0;
	tss.cs = 0x0b;
	tss.ds = 0x13;
	tss.es = 0x13;
	tss.fs = 0x13;
	tss.gs = 0x13;

}

void setTSSStack(u32 stack) {
	tss.esp0 = stack;
}

void gdt_init(){
	writeTSS();
    u8 kernelCode = ACCESS_RW | ACCESS_EXEC_CODE | ACCESS_INMEM | ACCESS_CODEORDATA;
    u8 kernelData = ACCESS_RW | ACCESS_INMEM | ACCESS_CODEORDATA;
    u8 userCode = ACCESS_USER | ACCESS_RW | ACCESS_EXEC_CODE | ACCESS_INMEM| ACCESS_CODEORDATA ;
    u8 userData = ACCESS_USER | ACCESS_RW | ACCESS_INMEM | ACCESS_CODEORDATA;
	u8 tssFlags = ACCESS_GDT_DESC|ACCESS_EXEC_CODE|ACCESS_USER|ACCESS_INMEM;
	gdt[0] = makeEntry(0, 0, 0, 0);
	gdt[1] = makeEntry(0xffffffff, 0, kernelCode, 0xCF);
	gdt[2] = makeEntry(0xffffffff, 0, kernelData, 0xCF);
    gdt[3] = makeEntry(0xffffffff, 0, userCode, 0xCF);
    gdt[4] = makeEntry(0xffffffff, 0, userData, 0xCF);
	gdt[5] = makeEntry(sizeof(TSSEntry), (u32)&tss, tssFlags, 0);


    loadGdt();
	tss_flush();
}