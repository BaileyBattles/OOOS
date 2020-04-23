#ifndef __ELFLOADER_H__
#define __ELFLOADER_H__

#include "Kernel/Types.h"

#define EI_NIDENT 16
typedef u32	    Elf32_Addr;
typedef u16	    Elf32_Half;
typedef u32	    Elf32_Off;
typedef s32		Elf32_Sword;
typedef u32	    Elf32_Word;
typedef u64	    Elf32_Lword;

typedef struct {
	unsigned char	e_ident[EI_NIDENT];	/* File identification. */
	Elf32_Half	    e_type;		/* File type. */
	Elf32_Half	    e_machine;	/* Machine architecture. */
	Elf32_Word	    e_version;	/* ELF format version. */
	Elf32_Addr	    e_entry;	/* Entry point. */
	Elf32_Off	    e_phoff;	/* Program header file offset. */
	Elf32_Off	    e_shoff;	/* Section header file offset. */
	Elf32_Word	    e_flags;	/* Architecture-specific flags. */
	Elf32_Half  	e_ehsize;	/* Size of ELF header in bytes. */
	Elf32_Half	    e_phentsize;	/* Size of program header entry. */
	Elf32_Half	    e_phnum;	/* Number of program header entries. */
	Elf32_Half	    e_shentsize;	/* Size of section header entry. */
	Elf32_Half	    e_shnum;	/* Number of section header entries. */
	Elf32_Half	    e_shstrndx;	/* Section name strings section. */
} Elf32_Ehdr;


struct ELFInfo {
    u32 entryAddress;
};


class ELFLoader {
public:
    ELFInfo load(const char path[]);
private:

};

#endif