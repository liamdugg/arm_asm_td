.code 32

/* ----- GLOBAL VARIABLES ----- */
.global _start

/* --------- INCLUDES --------- */
.include "asm_equ.h"

/* ----- EXTERN VARIABLES ----- */

.extern _main

.extern sys_init
.extern _paging_init
.extern _paging_enable
.extern _sch_init

.extern _RAM_BASE
.extern _KERNEL_SIZE
.extern _KERNEL_LMA_START

.extern _VTBL_SIZE
.extern _VTBL_LMA_START
.extern _VTBL_VMA_START

/* MACRO */
.macro macro_irq_ena
    DSB
    cpsie i @ haciendo solo esto no andaba
    DSB
    ISB
.endm

/* ----- SECTION: STARTUP ----- */
.section .startup, "a"
_start:

    @ copia de kernel a RAM
    ldr r0, = _KERNEL_LMA_START
    ldr r1, = _RAM_BASE             
    ldr r2, = _KERNEL_SIZE     
    bl _memcopy     
    
    @ copia de vector table a 0x0    
    ldr r0, = _VTBL_LMA_START
    ldr r1, = _VTBL_VMA_START
    ldr r2, = _VTBL_SIZE   
    bl _memcopy

    bl _paging_init     @ configuro paginacion
    bl sys_init         @ init timer y gic
    bl _paging_enable   @ habilito la paginacion
    bl _sch_init
    
    macro_irq_ena       @ habilito irq

    b _main             @ salto al main del kernel

_memcopy:
    add r2,r2,r0
    _copy:
        ldmia r0!, {r3 - r10};
        stmia r1!, {r3 - r10};
        cmp r0,r2
        ble _copy
    movs pc, lr

.end
