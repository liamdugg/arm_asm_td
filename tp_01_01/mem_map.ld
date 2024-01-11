OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

_ROM_BASE = 0x70010000;
_RAM_BASE = 0x70030000;
_STACK_BASE = 0x70020000;

MEMORY {
    public_rom      : ORIGIN = _ROM_BASE, LENGTH = 0x1000
    public_stack    : ORIGIN = _STACK_BASE, LENGTH = 0x1000
    public_ram      : ORIGIN = _RAM_BASE, LENGTH = 0x1000
}

SECTIONS {

    . = _ROM_BASE;

    .text : {
        *(.text*)
    } > public_rom

    _KERNEL_VMA_START = _RAM_BASE;
    _KERNEL_LMA_START = LOADADDR(.text) + SIZEOF(.text);

    .kernel _KERNEL_VMA_START : AT(_KERNEL_LMA_START) {
        *(.kernel)
    } > public_ram
    _KERNEL_SIZE = SIZEOF(.kernel);

    .data : {*(.data*)} > public_ram
    
    .bss : {
        . = ALIGN(4);
        _BSS_START = .;
        *(.bss*)
        _BSS_END = .;
    } > public_ram
}