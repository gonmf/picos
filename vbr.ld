OUTPUT_FORMAT("binary")
SECTIONS
{
  .vbr 0x7C00 : {
    *(.text)
    *(.rodata)
    *(.data)
    . = 510;
    SHORT(0xAA55)
  }
}
