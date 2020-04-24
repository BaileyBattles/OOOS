#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "Kernel/Types.h"
#include "Memory/Paging.h"
#include "Process/ELFLoader.h"
#include "Process/IPC.h"

#define MAX_NUM_PROCESSES 100
#define STACK_SIZE 10*KB

#define KERNEL_LEVEL 0
#define USER_LEVEL   3

static Process *currentProcess;

struct PCB {
	u32 eip;
	u32 esp;
	u32 ebp;
};

class Keyboard;

class Process {
public:
	friend class IPCSocket;

	static Process createInitProcess(void (*func)(Process *));
	Process createChildProcess(const char path[], int level, bool user);
    void exec();

    PagingStructure *getPagingStructure();
	void connectToKeyboard(Keyboard *keyboard);
	void readFromIPC();


private:
    char *path;
	ELFLoader elfLoader;
	PCB pcb;
	Process *parent;
	bool isUserMode;
    
	
	IPCSocket socket;
	static void enterUserMode(u32 entryAddress);
    PagingStructure pagingStructure;
    int loadElf(const char path[]);
	void storeRegisters(PCB &processControlBlock);
	void storeInitRegisters(PCB &processControlBlock);


};


#endif