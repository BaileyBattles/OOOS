#include "CPU/InterruptManager.h"
#include "Drivers/Screen.h"
#include "Util/String.h"
#include "Kernel/Types.h"
#include "Memory/KMemoryManager.h"
#include "Memory/Paging.h"
#include "Process/Process.h"
#include "Util/Memcpy.h"

extern "C" u32 s_stack;
extern "C" u32 e_stack;

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

void PageTableManager::mapNewPagingStructure(PagingStructure &newStructure, PagingStructure &oldStructure) {
    u32 address = (u32)newStructure.pageDirectoryPtr;
    mapPage(&oldStructure, address, address,false);
    for (int i = 0; i < NUM_PAGETABLES; i++) {
        u32 address = (u32)newStructure.pageTablePtrs[i];
        mapPage(&oldStructure, address, address,false);
    }
    // memory_set(newStructure.pageDirectoryPtr, '\0', PAGEMALLOC_SIZE);
    // for (int i = 0; i < NUM_PAGETABLES; i++) {
    //     memory_set(newStructure.pageTablePtrs[i], '\0', PAGEMALLOC_SIZE);
    // }
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
    //Initialize the new paging structure
    PagingStructure structure;
    structure.pageDirectoryPtr = (PageDirectory *)KMM.pagemallocPhysical();
    for (int i = 0; i < NUM_PAGETABLES; i++) {
        structure.pageTablePtrs[i] = (PageTable *)KMM.pagemallocPhysical();
    }  

    //map the new paging structure in
    if (pagingInitialized)
        mapNewPagingStructure(structure, pagingStructure);
    
    //Write to the new pagetable
    for (int i = 0; i < NUM_PAGETABLES; i++) {
        setPDEPresent(structure.pageDirectoryPtr->entry[i]);
        setPDEWriteable(structure.pageDirectoryPtr->entry[i]);
        setPDEUserMode(structure.pageDirectoryPtr->entry[i]);
        setPDEBaseAddress(structure.pageDirectoryPtr->entry[i], 
                                (u32)structure.pageTablePtrs[i]);
    }

    //Map the kernel
    if (pagingInitialized) {
        preserveKernelMapping(structure, pagingStructure);
    }
    else {
        initializeKernelMapping(structure);
        mapNewPagingStructure(structure, structure);
    }

    //Map the new userland
    int numPhysicalPages = TOTAL_MEMORY / 4096;
    // for (int i = 0; i < numPhysicalPages / 2; i++) {
    //     u32 offset = i*4096;
    //     mapPage(&structure, USERSPACE_START_VIRTUAL + offset, offset + (TOTAL_MEMORY / 2), true);
    // }

    return structure;
}

void PageTableManager::copyMemory(PagingStructure *oldStructure, PagingStructure *newStructure) {

    //Userland stuff
    for (int i = USERSPACE_START_VIRTUAL; i < USERSPACE_START_VIRTUAL + 0x1000; i += (PAGE_SIZE)) {
        PageTableEntry *entry = getPageTableEntry(oldStructure, i);
        if (ptePresent(*entry)) {
            void *physicalAddress = KMM.pagemallocPhysical(1);
            mapPage(oldStructure, 0x90000000, (u32)physicalAddress, true);
            flush_tlb();
            memory_copy((void*)i, (void*)0x90000000, PAGE_SIZE);
            mapPage(newStructure, i, (u32)physicalAddress, true);
        }
    }
    for (int i = USERSPACE_START_VIRTUAL + 0x1000; i < USERSPACE_START_VIRTUAL + 0x10f0000; i+= (PAGE_SIZE)) {
        PageTableEntry *entry = getPageTableEntry(oldStructure, i);
        if (ptePresent(*entry)) {
            u32 physicalAddress = pteBaseAddress(*entry);
            mapPage(newStructure, i, physicalAddress, true);
        }
    }

    //Kernel Stuff
    for (int i = (int)&s_stack; i < (int)&e_stack; i += PAGE_SIZE) {
        void *physicalAddress = KMM.pagemallocPhysical(1);
        mapPage(oldStructure, 0x90000000, (u32)physicalAddress, false);
        flush_tlb();
        memory_set((void*)0x90000000, '/0', PAGE_SIZE);
        memory_copy((void*)i, (void*)0x90000000, PAGE_SIZE);
        mapPage(newStructure, i, (u32)physicalAddress, false);
    }
}

void PageTableManager::copySegment(PagingStructure *oldStructure, PagingStructure *newStructure, void* start, void *end) {
    void *startPageAddress = KMM.calculateNextAllignedAddress((u32)start, PAGE_SIZE) - PAGE_SIZE;
    void *endPageAddress = KMM.calculateNextAllignedAddress((u32)end, PAGE_SIZE) - PAGE_SIZE;
    
    // PageTableEntry *entry = getPageTableEntry(newStructure, (u32)startPageAddress);
    // u32 physicalAddress = pteBaseAddress(*entry);
    // mapPage(oldStructure, 0x90000000, physicalAddress, false);
    // flush_tlb();
    // u32 offset = (u32)start % (PAGE_SIZE);
    // u32 length = (u32)end - (u32)start;
    //memory_copy(start, (void*)(0x90000000 + offset), length);
    
    u32 currentAddress = (u32)start;
    while (currentAddress < (u32)end) {
        u32 nextPageAddress = (u32)KMM.calculateNextAllignedAddress(currentAddress, PAGE_SIZE);
        if (nextPageAddress == currentAddress) {
            nextPageAddress += PAGE_SIZE;
        }
        u32 length;
        if ((u32)nextPageAddress < (u32)end) {
            length = (u32)nextPageAddress - (u32)currentAddress;
        }
        else {
            length = (u32)end - (u32)currentAddress;
        }
        copySegmentOnSinglePage(oldStructure, newStructure, (void*)currentAddress, length);
        currentAddress += length;
    }
}

void PageTableManager::copySegmentOnSinglePage(PagingStructure *oldStructure, PagingStructure *newStructure, void* start, u32 length) {
    u32 startPageAddress = (u32)KMM.calculateNextAllignedAddress((u32)start, PAGE_SIZE) - PAGE_SIZE;
    if ((u32)start % (PAGE_SIZE) == 0) {
        startPageAddress += PAGE_SIZE;
    }
    PageTableEntry *entry = getPageTableEntry(newStructure, (u32)startPageAddress);
    u32 physicalAddress = pteBaseAddress(*entry);
    mapPage(oldStructure, 0x90000000, physicalAddress, false);
    flush_tlb();
    u32 offset = (u32)start % (PAGE_SIZE);
    memory_copy(start, (void*)(0x90000000 + offset), length);
}





PagingStructure PageTableManager::getCurrentPagingStructure() {
    return pagingStructure;
}

void PageTableManager::initialize() {
    KMM.mallocKernelPages();
    pagingStructure = initializeProcessPageTable();
    //Map first 256 GB of kernel to 0-256 GB physical

    registerISRHandler(this, 14);
    write_cr3((u32)pagingStructure.pageDirectoryPtr);
    initialize_cr0();
    pagingInitialized = true;
    //causeExamplePageFault();
}

int PageTableManager::mmap(void *virtualAddress, int length) {
    if ((u32)virtualAddress < USERSPACE_START_VIRTUAL) {
        kprint("mmap : Cannot MMAP into kernel space\n");
        return -1;
    }
    PageTableEntry *entry = getPageTableEntry(&pagingStructure, (u32)virtualAddress);
    if (ptePresent(*entry)) {
        kprint("mmap : Virtual Address already mapped\n");
        return -1;
    }
    u32 startAddress = (u32)KMemoryManager::calculateNextAllignedAddress((u32)virtualAddress, PAGE_SIZE);
    int numPages = length / (PAGE_SIZE) + 1;
    void *physicalAddress = KMM.pagemallocPhysical(numPages);

    for (int i = 0; i < numPages; i++) {
        int offset = i*4096;
        if (physicalAddress == nullptr) {
            kprint("mmap : Ran out of physical memory\n");
            return -1;
        }
        mapPage(&pagingStructure, (u32)virtualAddress + offset, (u32)physicalAddress + offset, true);
    }
    
}

void PageTableManager::pageTableSwitch(Process *process) {
    u32 *ebp; u32 esp;
    asm("\t movl %%esp,%0" : "=r"(esp));
    asm("\t movl %%ebp,%0" : "=r"(ebp));
    copySegment(&pagingStructure, process->getPagingStructure(), (void*)esp, (void*)(*ebp + 0x12));
    
    PagingStructure *structure = process->getPagingStructure();
    asm volatile("movl %%eax, %%cr3" ::"a"((u32)structure->pageDirectoryPtr)
                 : "memory");
    pagingStructure = *structure;
}

void PageTableManager::handleInterrupt(registers_t r) {
    u32 val = read_cr2();
    kprint("Page Fault\n");
}

void PageTableManager::initializeKernelMapping(PagingStructure& structure) {
    int numPhysicalPages = KMM.mallocKernelPages();
    for (int i = 0; i < numPhysicalPages; i++) {
        u32 offset = i*4096;
        mapPage(&structure, offset, offset, false);
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

void PageTableManager::flush_tlb() {
    u32 cr3 = read_cr3();
    write_cr3(cr3);
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