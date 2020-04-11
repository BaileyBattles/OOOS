ENTRY (loader)
SECTIONS
{
  . = 0x00100000;
  .mbheader : {
    *(.mbheader)
  }
  .text : {
    *(.text)
  }
  .rodata ALIGN (0x1000) : {
    start_ctors = .;
    *(.ctors)
    end_ctors = .;
    *(.rodata)
  }
  .data ALIGN (0x1000) : {
    *(.data)
  }
  .bss : {
    sbss = .;
    *(COMMON)
    *(.bss)
    ebss = .;
  }
}