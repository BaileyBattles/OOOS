#include "drivers/screen.h"
#include "util/string.h"
#include "kernel/types.h"
#include "memory/kMemoryManger.h"
#include "memory/paging.h"

void initializePageTableEntry(PageTableEntry entry, u32 data) {
    entry = data;
}

void initializePageDirectoryEntry(PageDirectoryEntry entry, u32 data) {
    entry = data;
}

//Page Table Functions
bool ptePresent(PageTableEntry entry){
    return entry & 0x1;
}

bool pteWriteable(PageTableEntry entry){
    return entry & 0x2;
}

bool pteUserMode(PageTableEntry entry){
    return entry & 0x4;
}

bool pteWritethrough(PageTableEntry entry){
    return entry & 0x8;
}

bool pteAccessed(PageTableEntry entry){
    return entry & 0x20;
}

bool pteDirty(PageTableEntry entry){
    return entry & 0x40;
}

u32 pteBaseAddress(PageTableEntry entry){
    return ((entry & 0x7FFFF000) >> 12);
}

void setPTEPresent(PageTableEntry &entry) {
    entry = entry | 0x1;
}

int setPTEBaseAddress(PageTableEntry &entry, u32 pteBaseAddress){
    entry = entry & ~0x7ffff000;
    entry = entry | pteBaseAddress;
}

//Page Directory Functions
bool pdePresent(PageDirectoryEntry entry){
    return entry & 0x1;
}
bool pdeWriteable(PageDirectoryEntry entry){
    return entry & 0x2;
}
bool pdeUserMode(PageDirectoryEntry entry){
    return entry & 0x4;
}
bool pdeAccessed(PageDirectoryEntry entry){
    return entry & 0x20;
}
bool pdeDirty(PageDirectoryEntry entry){
    return entry & 0x40;
}
bool pdeFourMB(PageDirectoryEntry entry){
    return entry & 0x80;
}
bool pdeBaseAddress(PageDirectoryEntry entry){
    return ((entry & 0x7FFFF000) >> 12);
}

void setPDEPresent(PageDirectoryEntry &entry){
    entry = entry | 0x1;
}

void setPDEWriteable(PageDirectoryEntry &entry){
    entry = entry | 0x2;
}

int setPDEBaseAddress(PageDirectoryEntry &entry, u32 pdeBaseAddress){
    entry = entry & ~0x7ffff000;
    entry = entry | pdeBaseAddress;
}

//////////////////////
// PageTableManager //
//////////////////////

u32 setContinuousPageTable(PageTable &pageTable, u32 baseAddress){
    u32 currentAddress = baseAddress;
    for (int i = 0; i < NUM_PAGETABLE_ENTRIES; i++){
        if (currentAddress == 0x30000000) {
            setPTEBaseAddress(pageTable.entry[i], 0x800000);
            setPTEPresent(pageTable.entry[i]);
        }
        else {
            setPTEBaseAddress(pageTable.entry[i], currentAddress);
            setPTEPresent(pageTable.entry[i]);
        }
        currentAddress += 4*KB;
    }
    return currentAddress;
}

void PageTableManager::initialize() {

    pageDirectory = (PageDirectory *)KMM.pagemalloc();

    u32 baseAddress = 0;


    for (int i = 0; i < NUM_PAGETABLES; i++) {
        pageTables[i] = (PageTable *)KMM.pagemalloc();
        baseAddress = setContinuousPageTable(*pageTables[i], baseAddress);
    }

    for (int i = 0; i < NUM_PAGETABLES; i++) {
        setPDEPresent(pageDirectory->entry[i]);
        setPDEWriteable(pageDirectory->entry[i]);
        setPDEBaseAddress(pageDirectory->entry[i], (u32)pageTables[i]);
    }

    u32 val = read_cr0();
    write_cr3((u32)pageDirectory);
    initialize_cr0();

    u32 *ptr = (u32*)0x30000002;
    *ptr = 25;
    u32 value = *ptr;
}

u32 PageTableManager::read_cr3()
{
    u32 val;
    __asm__("movl %%cr3,%0" : "=r"(val));
    return val;
}

u32 PageTableManager::read_cr0()
{
    u32 val;
    __asm__("movl %%cr0,%0" : "=r"(val));
    return val;
}

void PageTableManager::write_cr3(u32 cr3)
{
    asm volatile("movl %%eax, %%cr3" ::"a"(cr3)
                 : "memory");
}

void PageTableManager::initialize_cr0(){
    __asm__ __volatile__(
        "movl %%cr0, %%eax\n"
        "orl $0x80000001, %%eax\n"
        "movl %%eax, %%cr0\n" ::
            : "%eax", "memory");
}