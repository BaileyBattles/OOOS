#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "sys/io.h"
#include "util/string.h"

Keyboard::Keyboard()
{
    shiftDown = false;
}

void Keyboard::initialize()
{}

char Keyboard::accountForShift(char input) {
    if (shiftDown)
        return input - 32;
    return input;
}

char Keyboard::translateInput(u8 input) {
    switch (input)
    {
    //Shift
    case 42:
        shiftDown = true;
        return '\0';
    case 170:
        shiftDown = false;
        return '\0';
    
    //Tab
    case 15:
        return '\t';
    
    //Enter
    case 28:
        return '\n';

    //Top Row
    case 16:
        return accountForShift('q');
    case 17:
        return accountForShift('w');
    case 18:
        return accountForShift('e');
    case 19:
        return accountForShift('r');
    case 20:
        return accountForShift('t');
    case 21:
        return accountForShift('y');
    case 22:
        return accountForShift('u');
    case 23:
        return accountForShift('i');
    case 24:
        return accountForShift('o');
    case 25:
        return accountForShift('p');

    //Second Row
    case 30:
        return accountForShift('a');
    case 31:
        return accountForShift('s');
    case 32:
        return accountForShift('d');
    case 33:
        return accountForShift('f');
    case 34:
        return accountForShift('g');
    case 35:
        return accountForShift('h');
    case 36:
        return accountForShift('j');
    case 37:
        return accountForShift('k');
    case 38:
        return accountForShift('l');
    
    //Third Row
    case 44:
        return accountForShift('z');
    case 45:
        return accountForShift('x');
    case 46:
        return accountForShift('c');
    case 47:
        return accountForShift('v');
    case 48:
        return accountForShift('b');
    case 49:
        return accountForShift('n');
    case 50:
        return accountForShift('m');

    //Invalid key or key that should not output anything like shift
    default:
        return '\0';
    }
}   

void Keyboard::handleInterrupt(registers_t r)
{
    u8 c = inb(0x60);
    char c1 = translateInput(c);
    char buff[2];
    buff[0] = c1;
    buff[1] = '\0';
    kprint(buff);
}