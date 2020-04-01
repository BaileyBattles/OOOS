#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "sys/io.h"
#include "util/string.h"

void Keyboard::initialize()
{}

void Keyboard::handleInterrupt(registers_t r)
{
    u8 c = inb(0x60);
    char buff[5]; 
    kprint(int_to_ascii(c, buff));
    kprint("\n");
}