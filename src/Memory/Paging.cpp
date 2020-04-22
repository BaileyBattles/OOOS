#include "CPU/InterruptManager.h"
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

void setPTENotPresent(PageTableEntry &entry) {
    entry = entry & ~0x1;
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

u32 PageTableManager::setContinuousPageTable(PageTable &pageTable, u32 baseAddress){
    u32 currentAddress = baseAddress;
    for (int i = 0; i < NUM_PAGETABLE_ENTRIES; i++){
        setPTEBaseAddress(pageTable.entry[i], currentAddress);
        setPTEPresent(pageTable.entry[i]);
        currentAddress += 4*KB;
    }
    return currentAddress;
}

void PageTableManager::mapPage(PagingStructure *structure, u32 virtualAddress, u32 physicalAddress) {
    PageTableEntry *pte = getPageTableEntry(structure, virtualAddress);
    setPTEBaseAddress(*pte, physicalAddress);
    setPTEPresent(*pte);
}

void causeExamplePageFault() {
    u32 *ptr = (u32*)(USERSPACE_START_VIRTUAL - 5);
    *ptr = 25;
}

PagingStructure PageTableManager::initializeProcessPageTable() {
    PagingStructure structure;
    structure.pageDirectoryPtr = (PageDirectory *)KMM.pagemalloc();

    for (int i = 0; i < NUM_PAGETABLES; i++) {
        structure.pageTablePtrs[i] = (PageTable *)KMM.pagemalloc();
    }    
    
    for (int i = 0; i < NUM_PAGETABLES; i++) {
        setPDEPresent(structure.pageDirectoryPtr->entry[i]);
        setPDEWriteable(structure.pageDirectoryPtr->entry[i]);
        setPDEBaseAddress(structure.pageDirectoryPtr->entry[i], 
                                (u32)structure.pageTablePtrs[i]);
    }
    int numPhysicalPages = TOTAL_MEMORY / 4096;

    for (int i = 0; i < numPhysicalPages / 2; i++) {
        u32 offset = i*4096;
        mapPage(&structure, KERNEL_START_VIRTUAL + offset, offset);
        mapPage(&structure, USERSPACE_START_VIRTUAL + offset, offset + (TOTAL_MEMORY / 2));
    }
    return structure;
}

void PageTableManager::initialize() {
    pagingStructure = initializeProcessPageTable();
    //Map first 256 GB of kernel to 0-256 GB physical

    registerISRHandler(this, 14);
    write_cr3((u32)pagingStructure.pageDirectoryPtr);
    initialize_cr0();

    //causeExamplePageFault();
}

void PageTableManager::handleInterrupt(registers_t r) {
    u32 val = read_cr2();
    kprint("Page Fault\n");
}

PageTableEntry *PageTableManager::getPageTableEntry(PagingStructure *structure, u32 virtualAddress) {
    u32 pdIndex = virtualAddress >> 22;
    PageDirectoryEntry pde = structure->pageDirectoryPtr->entry[pdIndex];
    PageTable *pageTable = (PageTable *)pdeBaseAddress(pde);
    u32 ptIndex = (virtualAddress >> 12) & 0x3FF;
    return &pageTable->entry[ptIndex];
}

u32 PageTableManager::physicalAddress(PagingStructure *structure, u32 virtualAddress) {
    PageTableEntry *pte = getPageTableEntry(structure, virtualAddress);
    u32 baseAddress = pteBaseAddress(*pte);
    return *pte + (virtualAddress & 0xFFF);
}

u32 PageTableManager::read_cr3()
{
    u32 val;
    __asm__("movl %%cr3,%0" : "=r"(val));
    return val;
}

u32 PageTableManager::read_cr2()
{
    u32 val;
    __asm__("movl %%cr2,%0" : "=r"(val));
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