#include "Drivers/Screen.h"
#include "Util/String.h"
#include "Kernel/Types.h"
#include "Memory/KMemoryManager.h"
#include "Memory/Paging.h"

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
    return (entry & 0x7FFFF000);
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
u32 pdeBaseAddress(PageDirectoryEntry entry){
    return (entry & 0x7FFFF000);
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

    pageDirectoryPtr = (PageDirectory *)KMM.pagemalloc();

    u32 baseAddress = 0;


    for (int i = 0; i < NUM_PAGETABLES; i++) {
        pageTablePtrs[i] = (PageTable *)KMM.pagemalloc();
        baseAddress = setContinuousPageTable(*pageTablePtrs[i], baseAddress);
    }

    for (int i = 0; i < NUM_PAGETABLES; i++) {
        setPDEPresent(pageDirectoryPtr->entry[i]);
        setPDEWriteable(pageDirectoryPtr->entry[i]);
        setPDEBaseAddress(pageDirectoryPtr->entry[i], (u32)pageTablePtrs[i]);
    }

    u32 val = read_cr0();
    write_cr3((u32)pageDirectoryPtr);
    initialize_cr0();

    u32 *ptr = (u32*)0x30000002;
    *ptr = 25;
    u32 value = *ptr;

    u32 e = physicalAddress((u32)ptr);
}

PageTableEntry PageTableManager::getPageTableEntry(u32 address) {
    u32 pdIndex = address >> 22;
    PageDirectoryEntry pde = pageDirectoryPtr->entry[pdIndex];
    PageTable *pageTable = (PageTable *)pdeBaseAddress(pde);
    u32 ptIndex = (address >> 12) & 0x3FF;
    return pageTable->entry[ptIndex];
}

u32 PageTableManager::physicalAddress(u32 virtualAddress) {
    PageTableEntry pte = getPageTableEntry(virtualAddress);
    u32 baseAddress = pteBaseAddress(pte);
    return pte + (virtualAddress & 0xFFF);
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