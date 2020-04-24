#include "FS/VFS.h"
#include "Process/ELFLoader.h"

ELFInfo ELFLoader::load(const char path[]) {
    char *buffer = (char*)0x1000000;
    ELFInfo info;
    int fd = VFS.open(path, 0);
    VFS.read(fd, buffer, 12652);
    Elf32_Ehdr header;
    memory_copy(buffer, (char *)&header, sizeof(Elf32_Ehdr));
    info.entryAddress = header.e_entry;

    Elf32_Shdr shdr;
    for (int i = 0; i < header.e_shnum; i++)
        memory_copy(buffer + header.e_shoff + i*sizeof(shdr), (char*)&shdr, sizeof(shdr));
    return info;
}