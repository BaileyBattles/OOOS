#include "CPU/InterruptManager.h"
#include "Drivers/Screen.h"
#include "Process/Process.h"
#include "Util/String.h"
#include "Kernel/Syscall.h"

extern "C"
int handle_syscall(int num, void *args, void *results) {
    return _syscall_table[num]((char*)args, (char*)results);
}

//Syscall Wrappers
int _kprint(char* buffer, char* result){
    kprint(buffer);
    return 0;
}

int _getInput(char* buffer, char* result) {
    result[0] = '\0';
    Process::currentProcess->theSocket()->read(result, 1);
    return 0;
}