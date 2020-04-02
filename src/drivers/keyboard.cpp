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

char Keyboard::translateInput(u8 input) {
    switch (input)
    {
    //Shift
    case 42:
        shiftDown = true;
        break;
    case 170:
        shiftDown = false;
        break;

    //Top Row
    case 16:
        if (shiftDown)
            return 'Q';
        else
            return 'q';
    case 17:
        return 'w';
    case 18:
        return 'e';
    case 19:
        return 'r';
    case 20:
        return 't';
    case 21:
        return 'y';
    case 22:
        return 'u';
    case 23:
        return 'i';
    case 24:
        return 'o';
    case 25:
        return 'p';

    //Second Row
    case 30:
        return 'a';
    case 31:
        return 's';
    case 32:
        return 'd';
    case 33:
        return 'f';
    case 34:
        return 'g';
    case 35:
        return 'h';
    case 36:
        return 'j';
    case 37:
        return 'k';
    case 38:
        return 'l';

    //Invalid key or key that should not output anything like shift
    default:
        return '\0';
    }
}   

void Keyboard::handleInterrupt(registers_t r)
{
    u8 c = inb(0x60);
    char c1 = translateInput(c);
    if (c1 == '\0'){
        char buff[5]; 
        kprint(int_to_ascii(c, buff));
        kprint("\n");
    }
    else {
        char buff[2];
        buff[0] = c1;
        buff[1] = '\0';
        kprint(buff);
        kprint("\n");
    }
    
}