OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset)

_ROM_BASE = 0x70010000;
_RAM_BASE = 0x70030000;

/* ----- STACK DEL KERNEL ----- */
_STACK_SIZE = 0x400; /* ver de achicarlo */
_STACK_BASE = 0x70020000;   

_SVC_STACK_BASE = _STACK_BASE     + _STACK_SIZE;
_IRQ_STACK_BASE = _SVC_STACK_BASE + _STACK_SIZE;
_FIQ_STACK_BASE = _IRQ_STACK_BASE + _STACK_SIZE;
_UND_STACK_BASE = _FIQ_STACK_BASE + _STACK_SIZE;
_ABT_STACK_BASE = _UND_STACK_BASE + _STACK_SIZE;

/* ----- MMU PAGINACION KERNEL ----- */
_PAGE_SIZE = 0x1000;
_TTL1_SIZE = 0x4000;
_TTL2_SIZE = 0x400;

_KERNEL_TABLES_START = 0x70040000;
_KERNEL_TABLES_SIZE  = (0x4000 + 64*_TTL2_SIZE);  /* 16k + 64*1k */
_KERNEL_TABLES_END   = _KERNEL_TABLES_START + _KERNEL_TABLES_SIZE; /* Terminan en 0x70054000 */

_KERNEL_TTL1_START   = _KERNEL_TABLES_START;
_KERNEL_TTL2_START   = _KERNEL_TTL1_START + _TTL1_SIZE;

/* ----- MMU PAGINACION TAREAS ----- */
_MAX_TASKS      = 3;
_TASK_CODE_SIZE = 0x2000;
_TASK_SIZE      = 0x10000;
_TASK_RAM_BASE  = 0x71000000; /* direccion en ram de primera tarea */ 

SECTIONS {

    . = _ROM_BASE;
    _STARTUP_START = _ROM_BASE;
    .startup _STARTUP_START : AT(_STARTUP_START) {
        *(.reset_handler*)
        *(.startup*)
    }
    
    _STARTUP_SIZE = SIZEOF(.startup);

    /* --------------------------------------------------- */
    
    _VTBL_VMA_START = 0x00000000;
    _VTBL_LMA_START = LOADADDR(.startup) + _STARTUP_SIZE;
    
    .vtbl _VTBL_VMA_START : AT(_VTBL_LMA_START) {
        *(.vtbl)
    } 

    _VTBL_SIZE = SIZEOF(.vtbl);

    /* --------------------------------------------------- */

    _KERNEL_LMA_START = LOADADDR(.startup) + _STARTUP_SIZE + _VTBL_SIZE;
    _KERNEL_VMA_START = _RAM_BASE;

    .kernel _KERNEL_VMA_START : AT(_KERNEL_LMA_START) ALIGN(0x4) {
        *(.vtbl_handlers*)
        *(.kernel*)
        *(.text*)
    } 

    _KERNEL_SIZE = SIZEOF(.kernel);

    /* --------------------------------------------------- */
    
    /*_DATA_LMA = LOADADDR(.kernel) + _KERNEL_SIZE;
    _DATA_VMA = _KERNEL_VMA_START + _KERNEL_SIZE;*/

    .data : {
        . = ALIGN(4096);
        _DATA_START = .;
        *(.data*)
        _DATA_END = .;
    } 

    /*_BSS_LMA = _DATA_LMA + SIZEOF(.data);
    _BSS_VMA = _DATA_VMA + SIZEOF(.data);*/

    .bss :{
        . = ALIGN(4096);
        _BSS_START = .;
        *(.bss*)
        *(COMMON*)
        _BSS_END = .;
    } 

    /* --------------------------------------------------- */

    /* TASK IDLE */
    _TASK_IDLE_LMA = LOADADDR(.bss) + SIZEOF(.bss);
    _TASK_IDLE_VMA = _TASK_RAM_BASE;
    
    .task_idle 0x80000000 : AT(_TASK_IDLE_LMA) {
        *(.task_idle*)
    }
    _TASK_IDLE_SIZE = SIZEOF(.task_idle);

    /* TASK 1*/
    _TASK1_LMA = _TASK_IDLE_LMA + _TASK_IDLE_SIZE;
    _TASK1_VMA = _TASK_RAM_BASE + _TASK_SIZE;

    .task1 0x80000000 : AT(_TASK1_LMA) {
        *(.task1)
    }
    _TASK1_SIZE = SIZEOF(.task1);

    /* TASK 2 */
    _TASK2_LMA = _TASK1_LMA + _TASK1_SIZE;
    _TASK2_VMA = _TASK1_VMA + _TASK_SIZE;

    .task2 0x80000000 : AT(_TASK2_LMA) {
        *(.task2*)
    }

    _TASK2_SIZE = SIZEOF(.task2);
}