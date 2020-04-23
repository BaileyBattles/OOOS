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
    edata = .;
  }
  .bss : {
    sbss = .;
    *(COMMON)
    *(.bss)
    /*All the static objects are of the form .bss._*.  Use readelf to see */
    *(.bss._*)

    ebss = .;
  }
}