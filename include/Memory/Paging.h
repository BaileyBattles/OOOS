#ifndef __PAGING_H__
#define __PAGING_H__

#include "CPU/InterruptHandlingObject.h"
#include "CPU/InterruptManager.h"
#include "Kernel/Types.h"

//THIS IS TERRIBLE!!!!
//Need to fix this with info from GRUB
#define TOTAL_MEMORY 0x20000000

#define KERNEL_START_VIRTUAL 0
#define USERSPACE_START_VIRTUAL 0x80000000

#define PAGE_SIZE 4*KB
#define NUM_PAGETABLE_ENTRIES 1024
#define NUM_PAGEDIR_ENTRIES 1024

#define NUM_PAGETABLES 1024

typedef u32 PageTableEntry;
typedef u32 PageDirectoryEntry;
typedef struct {
    PageDirectoryEntry entry[NUM_PAGETABLES];
} PageDirectory;

typedef struct {
    PageTableEntry entry[NUM_PAGETABLE_ENTRIES];
} PageTable;

typedef struct {
    PageDirectory *pageDirectoryPtr;
    PageTable *pageTablePtrs[NUM_PAGETABLES];
} PagingStructure;

void initializePageTableEntry(PageTableEntry entry, u32 data);
void initializePageDirectoryEntry(PageDirectoryEntry entry, u32 data);

//Page Table Functions
//Getters
bool ptePresent(PageTableEntry entry);
bool pteWriteable(PageTableEntry entry);
bool pteUserMode(PageTableEntry entry);
bool pteWritethrough(PageTableEntry entry);
bool pteCached(PageTableEntry entry);
bool pteAccessed(PageTableEntry entry);
bool pteDirty(PageTableEntry entry);
u32 pteBaseAddress(PageTableEntry entry);

//Setters
void setPTEPresent(PageTableEntry &entry);
void setPTENotPresent(PageTableEntry &entry);
void setPTEUserMode(PageTableEntry &entry);
void setPTEWriteable(PageTableEntry &entry);
//returns 0 on success, -1 on failure
int setPTEBaseAddress(PageTableEntry &entry, u32 pteBaseAddress);

//Page Directory Functions
bool pdePresent(PageDirectoryEntry entry);
bool pdeWriteable(PageDirectoryEntry entry);
bool pdeUserMode(PageDirectoryEntry entry);
bool pdeAccessed(PageDirectoryEntry entry);
bool pdeDirty(PageDirectoryEntry entry);
bool pdeFourMB(PageDirectoryEntry entry);
u32 pdeBaseAddress(PageDirectoryEntry entry);
//returns 0 on success, -1 on failure
void setPDEPresent(PageDirectoryEntry &entry);
void setPDEWriteable(PageDirectoryEntry &entry);
void setPDEUserMode(PageDirectoryEntry &entry);
int setPDEBaseAddress(PageDirectoryEntry &entry, u32 pdeBaseAddress);


class Process;

#define PTM PageTableManager::the()

class PageTableManager : public InterruptHandlingObject{
public:
    virtual void initialize();
    virtual void handleInterrupt(registers_t r);
    static PageTableManager& the() {
        static PageTableManager instance;
        return instance;
    }
    PagingStructure initializeProcessPageTable();
    void mapPage(PagingStructure *structure, u32 virtualAddress, u32 physicalAddress, bool user);
   
    //this switches the Addressing to the process's paging strucutre
    void pageTableSwitch(Process* process);

private:
    PageTableManager(){pagingInitialized = false;}
    bool pagingInitialized;
    void initialize_cr0();
    void write_cr3(u32 cr3);
    void makeIdentityMapping(u32 start, u32 length);

    u32 read_cr3();
    u32 read_cr2();
    u32 read_cr0();

    PagingStructure pagingStructure;
    u32 setContinuousPageTable(PageTable &pageTable, u32 baseAddress);
    PageTableEntry *getPageTableEntry(PagingStructure *structure, u32 virtualAddress);
    u32 physicalAddress(PagingStructure *structure, u32 virtualAddress);
    
    //Map the new paging structure into the kernel
    void mapNewPagingStructure(PagingStructure &newStructure, PagingStructure &oldStructure);
    void preserveKernelMapping(PagingStructure& newStructure, PagingStructure& oldStructure);
    void initializeKernelMapping(PagingStructure& structure);

};

#endif