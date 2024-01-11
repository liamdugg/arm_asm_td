.code 32

/* ----- EXTERNS ----- */
.extern _ROM_BASE
.extern _RAM_BASE
.extern _STACK_BASE
.extern _KERNEL_SIZE

/* ----- GLOBALS ----- */
.global _start
.global _main

.section .text
_start:

    ldr sp, = _STACK_BASE;          // init del stack pointer

    ldr r0, = _KERNEL_LMA_START     // origen 
    ldr r1, = _RAM_BASE             // destino = 0x70030000
    ldr r2, = _KERNEL_SIZE          // tamano de datos a copiar
    
    bl _memcopy                     // branch a la funcion de copia
    b _main                         // aca salto a donde este _main

_memcopy:
    add r2,r2,r0                    // r2 = _KERNEL_LMA_START + _KERNEL_SIZE
    _copy:
        ldmia r0!, {r3 - r10};
        stmia r1!, {r3 - r10};
        cmp r0,r2
        ble _copy
    mov pc, lr

.section .kernel, "a"
_main:
    wfi @ me quedo en bajo consumo
.end
