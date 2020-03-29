ENTRY(_start)
SECTIONS {
	. = 0x7c00;
	.text :
	{
		*(.boot)
		*(.text)
	}
    .rodata :
    {
        *(.rodata)
    }
    .data :
    { 
        *(.data)
    }
    .bss :
    {
        *(.bss)
    }
    . = 0x16000;
    .stack :
    {
        *(.stack)
    }
}