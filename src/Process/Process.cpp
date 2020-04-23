#include "FS/VFS.h"
#include "Memory/KMemoryManager.h"
#include "Process/Process.h"
#include "Util/String.h"

Process::Process(const char thePath[]) {
    pagingStructure = PageTableManager::the().initializeProcessPageTable();
    path = (char *)KMM.kmalloc(strlen(thePath) + 1);
    memory_copy(thePath, path, strlen(thePath) + 1);
}

PagingStructure* Process::getPagingStructure() {
    return &pagingStructure;
}

void Process::exec() {
    char *buff = (char*)0x1000000;
    ELFInfo elfInfo = elfLoader.load(path, buff);
    PageTableManager::the().pageTableSwitch(this);
    u32 stackPointer;
    u32 basePointer;
    asm("\t movl %%esp,%0" : "=r"(stackPointer));
    u32 newSp = USERSPACE_START_VIRTUAL + 0x1000;
    asm volatile("movl %%eax, %%esp" ::"a"(newSp)
                 : "memory");
    ((void (*)(void))elfInfo.entryAddress)();
    asm volatile("movl %%eax, %%esp" ::"a"(stackPointer)
                 : "memory");
}