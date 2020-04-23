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
    return info;
}