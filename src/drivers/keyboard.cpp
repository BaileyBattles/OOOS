#include "drivers/keyboard.h"
#include "sys/io.h"

void Keyboard::initialize()
{}

void Keyboard::handleInterrupt(registers_t r)
{
    inb(0x60);
}