#include "drivers/screen.h"
#include "sys/interrupt.h"
#include "sys/interrupt_constants.h"
#include "sys/io.h"
#include "util/atoi.h"


extern void isr_handler(InterruptRegisters_t reg) {
    print("Received int ");
    print(itoa(reg.int_no));
    print("\n");
}


/////////////////////
// InteruptManager //
////////////////////

InterruptManager::InterruptManager() {
    initializeIdtGates();
    initializeIdtPointer();
    remapPIC(32, 64);
    __asm__ __volatile__ ("sti");
}

//Setup one entry in the IDT
void InterruptManager::initializeIdtEntry(struct IdtEntry *idtEntry, u32 base,
                        u16 segmentSelector, u8 flags) {
    idtEntry->offsetLow = base & 0xFFFF;
    idtEntry->offsetHigh = (base >> 16) & 0xFFFF;
    idtEntry->segmentSelector = segmentSelector;
    idtEntry->alwaysZero = 0;
    idtEntry->flags = flags;
}

//Initialize and load the IdtPointer
void InterruptManager::initializeIdtPointer() {
    u32 base = (u32)&idtEntries[0];
    u16 limit = NUM_IDT_ENTRIES * sizeof(IdtEntry_t);
    idtPointer.limit = limit;
    idtPointer.base = base;
    load_idt((u32)&idtPointer);
}

void InterruptManager::remapPIC(u8 offset1, u8 offset2) {
    u8 a1 = 0xFF;
    u8 a2 = 0xFF;
 
	//a1 = inb(PIC1_DATA);                        // save masks
	//a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void InterruptManager::sendEOIToPIC(u8 irqNum){
    if (irqNum > NUM_OF_IRQ_ON_PIC1) 
		outb(PIC2_COMMAND,PIC_EOI);
    else
	    outb(PIC1_COMMAND,PIC_EOI);
}

//Initialize all the Entries
void InterruptManager::initializeIdtGates() {
    initializeIdtEntry(&idtEntries[0], (u32)isr0, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[1], (u32)isr1, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[2], (u32)isr2, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[3], (u32)isr3, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[4], (u32)isr4, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[5], (u32)isr5, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[6], (u32)isr6, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[7], (u32)isr7, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[8], (u32)isr8, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[9], (u32)isr9, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[10], (u32)isr10, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[11], (u32)isr11, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[12], (u32)isr12, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[13], (u32)isr13, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[14], (u32)isr14, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[15], (u32)isr15, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[16], (u32)isr16, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[17], (u32)isr17, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[18], (u32)isr18, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[19], (u32)isr19, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[20], (u32)isr20, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[21], (u32)isr21, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[22], (u32)isr22, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[23], (u32)isr23, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[24], (u32)isr24, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[25], (u32)isr25, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[26], (u32)isr26, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[27], (u32)isr27, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[28], (u32)isr28, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[29], (u32)isr29, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[30], (u32)isr30, 0x08, 0x8E);
    initializeIdtEntry(&idtEntries[31], (u32)isr31, 0x08, 0x8E);
}


