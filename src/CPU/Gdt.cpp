#include "CPU/Gdt.h"


gdtEntry encodeGdtEntry(u32 size, u32 baseAddr, u8 access,
		u8 flags) {
	//get me some of those GDTs
	gdtEntry toReturn;
	//lower part of lim
	toReturn.limit_1 = size & 0xFFFF;
	//lower base
	toReturn.base_1 = baseAddr & 0xFFFF;
	//mid base
	toReturn.base_2 |= ((baseAddr & 0xFF0000) >> 16);
	//who can get in
	toReturn.access = access;
	//upper limit FUCK YOU INTEL
	toReturn.limit_2_flags = (size & 0xF0000) >> 16;
	//flags. (attached to upper lim b/c intel)
	toReturn.limit_2_flags |= (flags << 4) & 0xF0;
	//upper base
	toReturn.base_3 = ((baseAddr & 0xFF000000) >> 24);
	return toReturn;
}

gdtEntry gdt[5];

void loadGdt(gdtEntry* toLoad, size_t lengthOfArr) {
	//get size, -1 because intell hates you
	size_t sizeOfGdt = lengthOfArr * sizeof(gdtEntry) - 1;
	//get the info to tell cpu about the GDT
	static gdtDiescriptor gdtd;
	gdtd.size = sizeOfGdt;
	gdtd.offset = (u32) toLoad;
	//load that shit
	asm ("LGDT %[gdt]" : : [gdt] "m" (gdtd));
}

void buildGdt(){
	//null. just rember you need this because intel hates you
	gdt[0] = encodeGdtEntry(0, 0, 0, 0);
	//code
	gdt[1] = encodeGdtEntry(0xffffffff, 0, 0x9A, 0xCF);
	//data
	gdt[2] = encodeGdtEntry(0xffffffff, 0, 0x92, 0xCF);
    
    loadGdt(gdt, 5);
}