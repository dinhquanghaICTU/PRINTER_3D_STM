ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
  } > FLASH

  _sidata = LOADADDR(.data);
  _etext = .;

  .data :
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > SRAM AT> FLASH

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > SRAM

  /* ===== Heap & Stack ===== */

  ._user_heap_stack :
  {
    . = ALIGN(8);
    _end = .;
    PROVIDE(end = .);

    . = . + 0x400;   /* 1KB heap */
    . = ALIGN(8);
  } > SRAM
}
