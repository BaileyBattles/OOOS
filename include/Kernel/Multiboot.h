#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include "Kernel/Types.h"

typedef struct {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    //... and a bunch of other stuff, check below for more info
    //https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
}__attribute__((packed)) multiboot_header_t;

#endif