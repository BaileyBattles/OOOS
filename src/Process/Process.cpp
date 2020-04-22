#include "FS/VFS.h"
#include "Process/Process.h"

Process::Process(const char path[]) {
    pagingStructure = PageTableManager::the().initializeProcessPageTable();
    int fd = VFS.open(path, 0);
    char *buff = (char*)0x1000000;
    VFS.read(fd, buff, 12652);
}

const PagingStructure* Process::getPagingStructure() {
    return &pagingStructure;
}

void Process::exec() {
    PageTableManager::the().pageTableSwitch(this);
    ((void (*)(void))0x1001000)();
}