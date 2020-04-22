#include "FS/VFS.h"
#include "Process/Process.h"

Process::Process(const char path[]) {
    VFS.open(path, 0);
    int fd = VFS.open(path, 0);
    char *buff = (char*)0x1000000;
    VFS.read(fd, buff, 12652);
}

void Process::exec() {
    ((void (*)(void))0x1001000)();
}