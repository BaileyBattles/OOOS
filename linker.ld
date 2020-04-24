ENTRY (loader)
SECTIONS
{
  . = 0x00100000;
  .text ALIGN (4K):AT (ADDR(.text))  {
    *(.mbheader)
    *(.text)
  }
  .rodata ALIGN (4K) : {
    start_ctors = .;
    *(.ctors)
    end_ctors = .;
    *(.rodata)
  }
  .data ALIGN (4K) : {
    *(.data)
    edata = .;
  }
  .bss ALIGN (4K) : {
    sbss = .;
    *(COMMON)
    *(.bss)
    /*All the static objects are of the form .bss._*.  Use readelf to see */
    *(.bss._*)

    ebss = .;
  }
}