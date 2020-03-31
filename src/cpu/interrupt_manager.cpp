#include "cpu/interrupt_constants.h"
#include "cpu/interrupt_manager.h"
#include "cpu/idt.h"
#include "drivers/screen.h"
#include "sys/io.h"
#include "util/string.h"
#include "util/memcpy.h"

InterruptHandler interruptHandlers[256];


///////////////////////
// Interrupt Manager //
///////////////////////

InterruptManager::InterruptManager() {
    setupIdtGates();
    remapPIC(0x20, 0x28);
    setupIrqGates();
    enableHardwareInterrupts();
}

void InterruptManager::setupIdtGates() {
    set_idt_gate(0, (u32)isr0);
    set_idt_gate(1, (u32)isr1);
    set_idt_gate(2, (u32)isr2);
    set_idt_gate(3, (u32)isr3);
    set_idt_gate(4, (u32)isr4);
    set_idt_gate(5, (u32)isr5);
    set_idt_gate(6, (u32)isr6);
    set_idt_gate(7, (u32)isr7);
    set_idt_gate(8, (u32)isr8);
    set_idt_gate(9, (u32)isr9);
    set_idt_gate(10, (u32)isr10);
    set_idt_gate(11, (u32)isr11);
    set_idt_gate(12, (u32)isr12);
    set_idt_gate(13, (u32)isr13);
    set_idt_gate(14, (u32)isr14);
    set_idt_gate(15, (u32)isr15);
    set_idt_gate(16, (u32)isr16);
    set_idt_gate(17, (u32)isr17);
    set_idt_gate(18, (u32)isr18);
    set_idt_gate(19, (u32)isr19);
    set_idt_gate(20, (u32)isr20);
    set_idt_gate(21, (u32)isr21);
    set_idt_gate(22, (u32)isr22);
    set_idt_gate(23, (u32)isr23);
    set_idt_gate(24, (u32)isr24);
    set_idt_gate(25, (u32)isr25);
    set_idt_gate(26, (u32)isr26);
    set_idt_gate(27, (u32)isr27);
    set_idt_gate(28, (u32)isr28);
    set_idt_gate(29, (u32)isr29);
    set_idt_gate(30, (u32)isr30);
    set_idt_gate(31, (u32)isr31);
}

void InterruptManager::remapPIC(u8 offset1, u8 offset2){
    // Initialize
    outb(MASTER_PIC_COMMAND, 0x11);
    outb(SLAVE_PIC_COMMAND, 0x11);

    //Set Offset
    outb(MASTER_PIC_DATA, offset1);
    outb(SLAVE_PIC_DATA, offset2);

    outb(MASTER_PIC_DATA, 0x04);
    outb(SLAVE_PIC_DATA, 0x02);
    outb(MASTER_PIC_DATA, 0x01);
    outb(SLAVE_PIC_DATA, 0x01);

    //write masks
    outb(MASTER_PIC_DATA, 0x01);
    outb(SLAVE_PIC_DATA, 0x0); 
}

void InterruptManager::enableHardwareInterrupts(){
    __asm__ __volatile__("sti");
}

void InterruptManager::setupIrqGates(){
    // Install the IRQs
    set_idt_gate(32, (u32)irq0);
    set_idt_gate(33, (u32)irq1);
    set_idt_gate(34, (u32)irq2);
    set_idt_gate(35, (u32)irq3);
    set_idt_gate(36, (u32)irq4);
    set_idt_gate(37, (u32)irq5);
    set_idt_gate(38, (u32)irq6);
    set_idt_gate(39, (u32)irq7);
    set_idt_gate(40, (u32)irq8);
    set_idt_gate(41, (u32)irq9);
    set_idt_gate(42, (u32)irq10);
    set_idt_gate(43, (u32)irq11);
    set_idt_gate(44, (u32)irq12);
    set_idt_gate(45, (u32)irq13);
    set_idt_gate(46, (u32)irq14);
    set_idt_gate(47, (u32)irq15);

    set_idt();
}

///////////////////////////////////
// High Level Interrupt Handlers //
///////////////////////////////////


/*
    These are used to interface with the global variable
    interruptHandlers which is an array of function pointers
*/

void handleInterrupt(registers_t r){
    if(interruptHandlers[0] != 0)
        interruptHandlers[r.int_no](r);
}

// Interrupts should be registered to the value
// returned in r.int_no
void registerHandler(InterruptHandler handler, int num){
    interruptHandlers[num] = handler;
}


//////////////////////////////////
// Low Level Interrupt Handlers //
/////////////////////////////////

/*
    These are called from the int stubs in assembly
*/

extern "C" void isr_handler(registers_t r) {
    kprint("received interrupt: ");
    char s[3];
    int_to_ascii(r.int_no, s);
    kprint(s);
    kprint("\n");
}

// void register_interrupt_handler(u8 n, InterruptHandler handler) {
//     interrupt_handlers[n] = handler;
// }

extern "C" void irq_handler(registers_t r) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (r.int_no >= 40)
        outb(SLAVE_PIC_COMMAND, EOI_COMMAND); 
    //}/* slave */
    outb(MASTER_PIC_COMMAND, EOI_COMMAND); /* master */
    inb(0x60);
    kprint("Received IRQ");
    char buff[3];
    int_to_ascii(r.int_no, buff);
    kprint(buff);
    kprint("\n");

    //Call associated interrupt handler
    handleInterrupt(r);
}


