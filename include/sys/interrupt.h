#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "kernel/types.h"

//Constants
#define NUM_IDT_ENTRIES 256

//Structs
struct IdtEntry {
    u16 offsetLow;
    u16 segmentSelector;
    u8 alwaysZero;
    u8 flags;
    u16 offsetHigh;
}__attribute__((packed));
typedef struct IdtEntry IdtEntry_t;

struct IdtPointer{
    u16 limit;
    u32 base;
}__attribute__((packed));
typedef struct IdtPointer IdtPointer_t;

struct InterruptRegisters {
   u32 ds; /* Data segment selector */
   u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
   u32 int_no, err_code; /* Interrupt number and error code (if applicable) */
   u32 eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
};
typedef struct InterruptRegisters InterruptRegisters_t;

//InterruptManager Class
class InterruptManager {
public:
    InterruptManager();
private:
    void initializeIdtGates();
    void initializeIdtEntry(struct IdtEntry *idtEntry, u32 base,
                        u16 segmentSelector, u8 flags);
    void initializeIdtPointer();
    void sendEOIToPIC(u8 irqNum);
    void remapPIC(u8 offset1, u8 offset2);

    IdtPointer_t idtPointer;
    IdtEntry_t idtEntries[NUM_IDT_ENTRIES];
};


//Extern functions defined in sys/interrupt_asm.asm
extern "C" void load_idt(u32 idtPointerAddress);
extern "C" void isr_handler(InterruptRegisters_t reg);

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
#endif