.code 32

/* ----- DEFINES ----- */
@ TODO: Poner en un .h
.equ MODE_USR,      0x10
.equ MODE_IRQ,      0x12
.equ MODE_SVC,      0x13

.equ IRQ_DISABLE,   0x80    @ bit 7
.equ FIQ_DISABLE,   0x40    @ bit 6

.equ IRQ_ENABLE,    0x0     @ no son necesarias, nada mas
.equ FIQ_ENABLE,    0x0     @ para hacer mas claro el codigo


/* ----- GLOBAL VARIABLES ----- */
.global _table
.global _reset
.global _undef_handler
.global _svc_handler
.global _prefetch_handler
.global _data_abort_handler
.global _irq_handler
.global _fiq_handler

/* ----- EXTERN VARIABLES ----- */
.extern _start
.extern _SVC_STACK_BASE

/* ----- SECTION: VECTOR TABLE ----- */
.section .vtbl, "a"
_table:
    ldr pc, = _reset
    ldr pc, = _undef_handler
    ldr pc, = _svc_handler
    ldr pc, = _prefetch_handler
    ldr pc, = _data_abort_handler
    nop
    ldr pc, = _irq_handler
_fiq_handler:
    b .

/* ----- SECTION: RESET HANDLERS----- */
.section .reset_handler, "a"
_reset:
    
    @ deshabilito interrupciones y pongo modo svc (es lo default pero lo hago igual)
    msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_SVC) @ _c para trabajar con bits 0:7
    ldr sp, = _SVC_STACK_BASE @ BASE = posicion mas alta

    @ no hay irqs ni fiqs, no necesito reactivarlas.
    b _start

/* ----- SECTION: VECTOR TABLE HANDLERS----- */
.section .vtbl_handlers, "a"
_svc_handler:
    stmfd sp!, {r0-r12, lr} @ guardo en stack
    
    ldr r0, [lr, #-4]           @ guardo instruccion "svc" en r0 (la anterior a la addr de lr)
    bic r0,r0,#0xFF000000       @ obtengo numero de svc.

    @ por ahora no hago nada con el numero, ya que no se pide.

    @ vuelvo de la interrupcion
    ldmfd sp!, {r0-r12, pc}^

@ unused handlers atm.
_undef_handler: 
    b .
_prefetch_handler: 
    b .
_data_abort_handler:
    b .
_irq_handler:
    b .

.end
