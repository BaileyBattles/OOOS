SECTIONS
{
    . = 0x1001000;
    .text ALIGN(0x1000): {
        *(.text)
    }
}