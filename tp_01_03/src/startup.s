.code 32

/* ----- GLOBAL VARIABLES ----- */
.global _start

/* ----- EXTERN VARIABLES ----- */
.extern _main
.extern hw_init

.extern _ROM_BASE
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

/* ----- SECTION: TEXT ----- */
.section .text
_start:

    @ copia de kernel a RAM
    ldr r0, = _KERNEL_LMA_START
    ldr r1, = _RAM_BASE             
    ldr r2, = _KERNEL_SIZE     
    bl _memcopy     
    
    @ copia de vector table a 0x00000000    
    ldr r0, = _VTBL_LMA_START
    ldr r1, = _VTBL_VMA_START
    ldr r2, = _VTBL_SIZE   
    bl _memcopy

    bl hw_init  @ init gic y timer
    macro_irq_ena

    @ para generar un undef instruction
    @ldr r0, =#0xFFFFFFFF
    @mov pc, r0

    @ corre el kernel
    b _main                         

_memcopy:
    add r2,r2,r0                    @ r2 = _KERNEL_LMA_START + _KERNEL_SIZE
    _copy:
        ldmia r0!, {r3 - r10};
        stmia r1!, {r3 - r10};
        cmp r0,r2
        ble _copy
    mov pc, lr

.end
