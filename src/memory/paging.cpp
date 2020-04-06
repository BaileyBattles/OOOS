#include "kernel/types.h"
#include "memory/paging.h"

////////////////////
// PageTableEntry //
////////////////////
PageTableEntry::PageTableEntry()
{}

PageTableEntry::PageTableEntry(u32 entry){
    data = entry;
}

bool PageTableEntry::isPresent() {
    return data & 0x1;
}

bool PageTableEntry::isWriteable(){
    return data & 0x2;
}

bool PageTableEntry::isUserMode(){
    return data & 0x4;
}

bool PageTableEntry::hasBeenAccessed(){
    return data & 0x20;
}

bool PageTableEntry::isDirty(){
    return data & 0x40;
}

u32 PageTableEntry::frameAddress(){
    return (data & 0x7FFFF000) >> 12;
}

u32 PageTableEntry::baseAddress(){
    return frameAddress() * PAGE_SIZE;
}

void PageTableEntry::setBaseAddress(u32 baseAddress){
    u32 frameAddress = baseAddress / PAGE_SIZE;
    data |= frameAddress << 12;
}

void PageTableEntry::setPresent() {
    data |= 0x1;
}


////////////////////////
// PageDirectoryEntry //
////////////////////////

PageDirectoryEntry::PageDirectoryEntry(u32 entry){
    data = entry;
}

bool PageDirectoryEntry::isPresent() {
    return data & 0x1;
}

bool PageDirectoryEntry::isWriteable(){
    return data & 0x2;
}

bool PageDirectoryEntry::isUserMode(){
    return data & 0x4;
}

bool PageDirectoryEntry::isCacheEnabled(){
    return data & 0x10;
}

bool PageDirectoryEntry::hasBeenAccessed(){
    return data & 0x40;
}

u32 PageDirectoryEntry::pageSize() {
    if (data & 0x80){
        return 4 * MB;
    }
    return 4 * KB;
}

u32 PageDirectoryEntry::frameAddress(){
    return (data & 0x7FFFF000) >> 12;
}

u32 PageDirectoryEntry::baseAddress(){
    return frameAddress() * PAGE_SIZE;
}

void PageTableManager::initialize() {
    initialize_cr0();
}

///////////////
// PageTable //
///////////////

//Create a whole table of continuous mapping
void PageTable::createContinuousMapping(u32 physicalStart, u32 virtualStart) {
    for (int i = 0; i < NUM_PAGETABLE_ENTRIES; i++){
        PageTableEntry pte;
        pte.setBaseAddress(physicalStart + i * PAGE_SIZE);
        pte.setPresent();
        u32 index = getIndex(virtualStart + i * PAGE_SIZE);
        entries[index] = pte;
    }
}

u32 PageTable::getIndex(u32 virtualAddress) {
    return ((virtualAddress >> 12) & 0x3FF);
}


///////////////////
// PageDirectory //
///////////////////

u32 PageDirectory::getIndex(u32 virtualAddress) {
    return ((virtualAddress >> 22) & 0x3FF);
}

//////////////////////
// PageTableManager //
//////////////////////

u32 PageTableManager::read_cr3()
{
    u32 val;
    __asm__("movl %%cr3,%0" : "=r"(val));
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

