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

#define SHT_PROGBITS 1

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

typedef struct elf32_phdr{
	Elf32_Word	p_type;
	Elf32_Off		p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
  	Elf32_Word	p_filesz;
  	Elf32_Word	p_memsz;
  	Elf32_Word	p_flags;
  	Elf32_Word	p_align;
} Elf32_Phdr;

typedef struct elf32_shdr {
  	Elf32_Word	sh_name;
  	Elf32_Word	sh_type;
  	Elf32_Word	sh_flags;
  	Elf32_Addr	sh_addr;
  	Elf32_Off	sh_offset;
  	Elf32_Word	sh_size;
  	Elf32_Word	sh_link;
  	Elf32_Word	sh_info;
  	Elf32_Word	sh_addralign;
  	Elf32_Word	sh_entsize;
} Elf32_Shdr;



struct ELFInfo {
    u32 entryAddress;
};


class ELFLoader {
public:
    ELFInfo load(const char path[]);
private:

};

#endif