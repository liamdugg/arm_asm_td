OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset)

_ROM_BASE = 0x70010000;
_RAM_BASE = 0x70030000;

/* ----- Stack related ----- */
_STACK_SIZE = 0xFFF;

_STACK_BASE = 0x70020000;
_SVC_STACK_BASE = 0x70020000;
_IRQ_STACK_BASE = _SVC_STACK_BASE + _STACK_SIZE;

MEMORY {
    public_vtbl     : ORIGIN = 0x00000000,  LENGTH = 0x1000 
    public_rom      : ORIGIN = _ROM_BASE,   LENGTH = 0x1000
    public_stack    : ORIGIN = _STACK_BASE, LENGTH = 0x1000
    public_ram      : ORIGIN = _RAM_BASE,   LENGTH = 0x1000
}

SECTIONS {

    . = _ROM_BASE;

    .text : {
        *(.reset_handler*)
        *(.text*)
    } > public_rom
    _TEXT_SIZE = SIZEOF(.text);

    /* --------------------------------------------------- */
    
    _VTBL_VMA_START = 0x00000000;
    _VTBL_LMA_START = LOADADDR(.text) + _TEXT_SIZE;

    .vtbl _VTBL_VMA_START : AT(_VTBL_LMA_START) {
        *(.vtbl)
    } > public_vtbl

    _VTBL_SIZE = SIZEOF(.vtbl);

    /* --------------------------------------------------- */

    _KERNEL_LMA_START = LOADADDR(.text) + _TEXT_SIZE + _VTBL_SIZE;

    .kernel _RAM_BASE : AT(_KERNEL_LMA_START) {
        *(.kernel*)
        *(.vtbl_handlers*)
    } > public_ram
    
    _KERNEL_SIZE = SIZEOF(.kernel);

    /* --------------------------------------------------- */

    .data : {*(.data*)} > public_ram
    
    .bss : {
        . = ALIGN(4);
        _BSS_START = .;
        *(.bss*)
        _BSS_END = .;
    } > public_ram
}