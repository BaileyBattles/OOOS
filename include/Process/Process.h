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


struct PCB {
	u32 eip;
	u32 esp;
	u32 ebp;
	u32 forkFlag;
	u32 yieldFlag;
};

class Keyboard;

class Process {
public:
	friend class IPCSocket;

	static Process createInitProcess(void (*func)(Process *));
	Process *createChildProcess(bool user);
    void exec(const char path[]);

    PagingStructure *getPagingStructure();
	void connectToKeyboard(Keyboard *keyboard);
	void readFromIPC();
	IPCSocket *theSocket();
	int getPID();



	void exit();
	void run();
	int fork();

	void reloadShell();

	PCB pcb;
private:
	static int nextPID;
	static int getNextPID();
    char *path;
	ELFLoader elfLoader;
	Process *parent;
	bool isUserMode;
	int pid;

	
	IPCSocket socket;
	static void enterUserMode(u32 entryAddress, PCB &pcb);
    PagingStructure pagingStructure;
    int loadElf(const char path[]);
	void storeRegisters(PCB &processControlBlock);
	void storeInitRegisters(PCB &processControlBlock);


};


#endif