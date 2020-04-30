#include "CPU/InterruptManager.h"
#include "Drivers/Screen.h"
#include "Util/String.h"
#include "Kernel/Types.h"
#include "Memory/KMemoryManager.h"
#include "Memory/Paging.h"
#include "Process/Process.h"
#include "Util/Memcpy.h"

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

void setPTEUserMode(PageTableEntry &entry){
    entry = entry | 0x4;
}

void setPTEWriteable(PageTableEntry &entry) {
    entry = entry | 0x2;
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

void setPDEUserMode(PageDirectoryEntry &entry){
    entry = entry | 0x4;
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

void PageTableManager::mapNewPagingStructure(PagingStructure &newStructure) {
    u32 address = (u32)newStructure.pageDirectoryPtr;
    mapPage(&pagingStructure, address, address,false);
    for (int i = 0; i < NUM_PAGETABLES; i++) {
        u32 address = (u32)newStructure.pageTablePtrs[i];
        mapPage(&pagingStructure, address, address,false);
    }
    memory_set(newStructure.pageDirectoryPtr, '\0', PAGEMALLOC_SIZE);
    for (int i = 0; i < NUM_PAGETABLES; i++) {
        memory_set(newStructure.pageTablePtrs[i], '\0', PAGEMALLOC_SIZE);
    }
}


void PageTableManager::mapPage(PagingStructure *structure, u32 virtualAddress, u32 physicalAddress, bool user ) {
    PageTableEntry *pte = getPageTableEntry(structure, virtualAddress);
    setPTEBaseAddress(*pte, physicalAddress);
    setPTEPresent(*pte);
    setPTEWriteable(*pte);
    if (user)
        setPTEUserMode(*pte);
}

void causeExamplePageFault() {
    u32 *ptr = (u32*)(USERSPACE_START_VIRTUAL - 5);
    *ptr = 25;
}

PagingStructure PageTableManager::initializeProcessPageTable() {
    PagingStructure structure;
    structure.pageDirectoryPtr = (PageDirectory *)KMM.pagemallocPhysical();

    for (int i = 0; i < NUM_PAGETABLES; i++) {
        structure.pageTablePtrs[i] = (PageTable *)KMM.pagemallocPhysical();
    }  
    if (pagingInitialized)  
        mapNewPagingStructure(structure);
    
    for (int i = 0; i < NUM_PAGETABLES; i++) {
        setPDEPresent(structure.pageDirectoryPtr->entry[i]);
        setPDEWriteable(structure.pageDirectoryPtr->entry[i]);
        setPDEUserMode(structure.pageDirectoryPtr->entry[i]);
        setPDEBaseAddress(structure.pageDirectoryPtr->entry[i], 
                                (u32)structure.pageTablePtrs[i]);
    }
    int numPhysicalPages = TOTAL_MEMORY / 4096;

    if (pagingInitialized)
        preserveKernelMapping(structure, pagingStructure);
    else
        initializeKernelMapping(structure);
    
    for (int i = 0; i < numPhysicalPages / 2; i++) {
        u32 offset = i*4096;
        mapPage(&structure, USERSPACE_START_VIRTUAL + offset, offset + (TOTAL_MEMORY / 2), true);
    }
    return structure;
}

void PageTableManager::initialize() {
    pagingStructure = initializeProcessPageTable();
    //Map first 256 GB of kernel to 0-256 GB physical

    registerISRHandler(this, 14);
    write_cr3((u32)pagingStructure.pageDirectoryPtr);
    initialize_cr0();
    pagingInitialized = true;
    //causeExamplePageFault();
}

void PageTableManager::pageTableSwitch(Process *process) {
    PagingStructure *structure = process->getPagingStructure();
    write_cr3((u32)structure->pageDirectoryPtr);
    PageTableEntry *entry = getPageTableEntry(structure, USERSPACE_START_VIRTUAL);
}

void PageTableManager::handleInterrupt(registers_t r) {
    u32 val = read_cr2();
    kprint("Page Fault\n");
}

void PageTableManager::initializeKernelMapping(PagingStructure& structure) {
    int numPhysicalPages = TOTAL_MEMORY / 4096;
    for (int i = 0; i < numPhysicalPages / 2; i++) {
        u32 offset = i*4096;
        if (offset >= 0x534000)
            return;
        mapPage(&structure, KERNEL_START_VIRTUAL + offset, offset, false);
    }
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

void PageTableManager::preserveKernelMapping(PagingStructure& newStructure,
                                             PagingStructure& oldStructure){
    for (int virtAddress = 0; virtAddress < USERSPACE_START_VIRTUAL; virtAddress += (PAGE_SIZE)){
        PageTableEntry *entry = getPageTableEntry(&oldStructure, virtAddress);
        if (ptePresent(*entry)) {
            PageTableEntry *newEntry = getPageTableEntry(&newStructure, virtAddress);
            *newEntry = *entry;
        }
    }                                            
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