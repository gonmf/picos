OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386:x86-64)
SECTIONS
{
   .start16 0x0000 : AT(0x10000) {
      start16.o (.text)
      start16.o (.rodata)
      start16.o (.data)
      . = NEXT(16);
   }

   .text   0x10000+SIZEOF(.start16) : AT(0x10000+SIZEOF(.start16)) { *(.text) }

   .rodata ALIGN(16) : { *(.rodata) }

   .data   ALIGN(16) : { *(.data) }

   .bss    ALIGN(16) : { 
      __bss_start = .;
      *(.bss)
      *(COMMON)
      . = ALIGN(16);
      __bss_end = .;
   }
   __bss_quads = SIZEOF(.bss)>>3;
}
