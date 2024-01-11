.code 32

/* ----- GLOBAL VARIABLES ----- */
.global _start

/* ----- EXTERN VARIABLES ----- */
.extern _main

.extern _ROM_BASE
.extern _RAM_BASE

.extern _KERNEL_SIZE
.extern _KERNEL_LMA_START

.extern _VTBL_SIZE
.extern _VTBL_LMA_START
.extern _VTBL_VMA_START

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
