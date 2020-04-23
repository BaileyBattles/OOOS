#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Process/Process.h"
#include "Util/String.h"

Process::Process(const char thePath[]) {
    pagingStructure = PageTableManager::the().initializeProcessPageTable();
    path = (char *)KMM.kmalloc(strlen(thePath) + 1);
    memory_copy(thePath, path, strlen(thePath) + 1);
}

const PagingStructure* Process::getPagingStructure() {
    return &pagingStructure;
}

void Process::exec() {
    loadElf(this->path);
    //PageTableManager::the().pageTableSwitch(this);
    u32 stackPointer;
    u32 basePointer;
    char* ex = (char*)USERSPACE_START_VIRTUAL + 0x1000;
    *ex = 'a';
    char ex2 = *ex;
    asm("\t movl %%esp,%0" : "=r"(stackPointer));
    // asm("\t movl %%ebp,%0" : "=r"(basePointer));

    u32 newSp = USERSPACE_START_VIRTUAL + 0x1000;
    asm volatile("movl %%eax, %%esp" ::"a"(newSp)
                 : "memory");
    ((void (*)(void))entryAddress)();
    // asm volatile("movl %%eax, %%esp" ::"a"(stackPointer)
    //              : "memory");
}

int Process::loadElf(const char path[]) {
    int fd = VFS.open(path, 0);
    char *buff = (char*)0x1000000;
    VFS.read(fd, buff, 12652);
    Elf32_Ehdr header;
    memory_copy(buff, (char *)&header, sizeof(Elf32_Ehdr));
    entryAddress = header.e_entry;
}