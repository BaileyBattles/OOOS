#ifndef GDT_H_
#define GDT_H_

#include "Kernel/Types.h"


typedef struct __attribute__((__packed__)) {
	u16 limit_1;
	u16 base_1;
	u8	base_2;
	u8 access;
	u8 limit_2_flags;
	u8 base_3;
} gdtEntry;

typedef struct __attribute__((__packed__)) {
	u16 size;
	u32 offset;
} gdtDiescriptor;



gdtEntry encodeGdtEntry(u32 maxAddr, u32 baseAddr, u8 access,
		u8 flags);

void loadGdt(gdtEntry* toLoad, size_t size);

void buildGdt();


#endif /* GDT_H_ */