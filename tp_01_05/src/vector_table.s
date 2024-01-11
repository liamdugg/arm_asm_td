.code 32

/* ----- INCLUDES -----  */
.include "asm_equ.h"

/* ----- GLOBAL FUNCTIONS ----- */
.global _table
.global _reset
.global _undef_handler
.global _svc_handler
.global _prefetch_handler
.global _data_abort_handler
.global _irq_handler
.global _fiq_handler

/* ----- EXTERN FUNCTIONS ----- */
.extern _start
.extern irq_handler

.extern _save_stacks
.extern _load_stacks
.extern _ttbr_switch

/* ----- EXTERN VARIABLES ----- */
.extern _UND_STACK_BASE
.extern _SVC_STACK_BASE
.extern _ABT_STACK_BASE
.extern _IRQ_STACK_BASE     
.extern _FIQ_STACK_BASE

/* ----- SECTION: VECTOR TABLE ----- */
.section .vtbl, "a"
_table:
    ldr pc, = _reset                // 0x0
    ldr pc, = _undef_handler        // 0x4
    ldr pc, = _svc_handler          // 0x8
    ldr pc, = _prefetch_handler     // 0xC
    ldr pc, = _data_abort_handler   // 0x10
    nop
    ldr pc, = _irq_handler          // 0x18
_fiq_handler:
    b .

/* ----- SECTION: RESET HANDLER ----- */
.section .reset_handler, "a"
_reset:

    @ Habria que deshabilitar la paginacion

    @ irq
    msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_IRQ)
    ldr sp, = _IRQ_STACK_BASE
    
    @ fiq
    msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_FIQ)
    ldr sp, = _FIQ_STACK_BASE

    @ abort
    msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_ABT)
    ldr sp, = _ABT_STACK_BASE

    @ undefined instruction
    msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_UND)
    ldr sp, = _UND_STACK_BASE

    @ svc
    msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_SVC)
    ldr sp, = _SVC_STACK_BASE

    b _start

/* ----- SECTION: VECTOR TABLE HANDLERS----- */
.section .vtbl_handlers, "a"
_svc_handler: 
    
    stmfd sp!, {r0-r12, lr}     @ guardo en stack              
    mrs r8, spsr            
    push {r8} 

    ldr r0, [lr, #-4]           @ guardo instruccion "svc" en r0 (la anterior a la addr de lr)
    bic r0,r0,#0xFF000000       @ obtengo numero de svc.

    @ Me fijo si llamo alguna de las dos tareas
    cmp   r0, #0xA1
    cmpne r0, #0xA2

    popeq {r8}                  @ saco el valor de spsr que habia cargado
    biceq r8, #0x1F             @ limpio los bits de modo
    orreq r8, #0x13             @ pongo en SVC
    pusheq {r8}                 @ vuelvo a guardar spsr
        
    pop {r8}
    msr spsr, r8

    ldmfd sp!, {r0-r12, pc}^

_irq_handler:
    
    sub lr, lr, #4
    add r10, r10, #1        @ contador
    
    stmfd sp!, {r0-r12, lr} @ LR se guarda en la posicion mas alta
    
    mrs r8, spsr
    push {r8}

    @mov r10, lr

    bl irq_handler

    @ Si hay que hacer context switch irq_handler devuelve 1 
    @ Si es la primera vez que entra a la irq devuelve 2 para poder setear stacks
    @ Si NO hay que hacer context switch devuelve 0.
    @ por la ABI se que el return me lo guarda en r0

    @ si es 2 cargo los stacks de la tarea idle al sistema
    cmp r0, #2
    bleq _load_stacks

    @ si es igual a 1 salto a hacer el context switch en ASM
    cmp r0, #1
    bleq _save_stacks
    bleq _ttbr_switch
    bleq _load_stacks

    pop {r8}
    msr spsr, r8

    ldmfd sp!, {r0-r12, pc}^

_undef_handler: 
    stmfd sp!, {r0-r12, lr}

    @ "UND" = 0x55 0x4E 0x44
    ldr r10,=#0x444E55

    ldmfd sp!, {r0-r12, pc}^

_prefetch_handler: 
    subs lr, lr, #8
    stmfd sp!, {r0-r12, lr}

    @ "PRE" = 0x50 0x52 0x45
    ldr r10,=#0x455250

    ldmfd sp!, {r0-r12, pc}^

_data_abort_handler:
    subs lr, lr, #8
    stmfd sp!, {r0-r12, lr}

    @ "ABT" = 0x41 0x42 0x54
    ldr r10,=#0x544241

    ldmfd sp!, {r0-r12, pc}^

.end
