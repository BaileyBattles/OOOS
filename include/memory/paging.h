#ifndef __PAGING_H__
#define __PAGING_H__

#include "kernel/types.h"

//THIS IS TERRIBLE!!!!
//Need to fix this with info from GRUB
#define TOTAL_MEMORY 0x20000000

#define PAGE_SIZE 4*KB
#define NUM_PAGETABLE_ENTRIES 1024
#define NUM_PAGEDIR_ENTRIES 1024

#define NUM_PAGETABLES 1024

class PageTableEntry {
public:
    PageTableEntry();
    PageTableEntry(u32 entry);
    bool isPresent();
    bool isWriteable();
    bool isUserMode();
    bool hasBeenAccessed();
    bool isDirty();
    u32 frameAddress(); //base address = frameAddress * 0x100
    u32 baseAddress(); // = frameAddress * 0x100

    void setPresent();
    void setBaseAddress(u32 baseAddress);

    u32 getData();
private:
    u32 data;
};

class PageDirectoryEntry {
public:
    PageDirectoryEntry(u32 entry);
    bool isPresent();
    bool isWriteable();
    bool isUserMode();
    bool isCacheEnabled();
    bool hasBeenAccessed();
    u32 pageSize();
    u32 frameAddress(); //base address = frameAddress * 0x100
    u32 baseAddress(); // = frameAddress * 0x100
private:
    u32 data;
};

class PageTable {
public:
    PageTable(){};
    PageTable(u32 *pageTableAddress);
    void createContinuousMapping(u32 physicalStart, u32 virtualStart);
    void setPageTableAddress(u32 *pageTableAddress);
private:
    u32 getIndex(u32 virtualAddress);
    u32 *pageTable;

    PageTableEntry entries[NUM_PAGETABLE_ENTRIES];
    void installEntry(u32 index, PageTableEntry pte);
};

class PageDirectory {

private:
    u32 getIndex(u32 virtualAddress);
};

class PageTableManager {
public:
    PageTableManager();
    void initialize();

private:
    void initialize_cr0();
    void write_cr3(u32 cr3);
    void makeIdentityMapping(u32 start, u32 length);
    u32 read_cr3();
    PageDirectory pageDirectory;
    PageTable *pageTables[NUM_PAGETABLES];
};

#endif