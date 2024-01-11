.code 32

/* ----- DEFINES ----- */
@ TODO: Poner en un .h
.equ MODE_USR,      0x10
.equ MODE_FIQ,      0x11
.equ MODE_IRQ,      0x12
.equ MODE_SVC,      0x13
.equ MODE_ABT,      0x17
.equ MODE_UND,      0x1B
.equ MODE_SYS,      0x1F

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
.extern irq_handler

.extern _UND_STACK_BASE     @ undefined
.extern _SVC_STACK_BASE
.extern _ABT_STACK_BASE     @ data abort
.extern _IRQ_STACK_BASE     
.extern _FIQ_STACK_BASE

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

/* ----- SECTION: RESET HANDLER ----- */
.section .reset_handler, "a"
_reset:

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

    @continuo ejecucion en modo svc
    b _start

/* ----- SECTION: VECTOR TABLE HANDLERS----- */
.section .vtbl_handlers, "a"
_svc_handler:
    stmfd sp!, {r0-r12, lr} @ guardo en stack
    
    ldr r0, [lr, #-4]           @ guardo instruccion "svc" en r0 (la anterior a la addr de lr)
    bic r0,r0,#0xFF000000       @ obtengo numero de svc.

    @ "SVC" = 0x53 0x56 0x43 --> guardo como 0x435653
    ldr r10, =#0x435653

    @ vuelvo de la interrupcion
    ldmfd sp!, {r0-r12, pc}^

_irq_handler:
    sub lr, lr, #4
    add r10, r10, #1  @ contador
    
    stmfd sp!, {r0-r12, lr}
    mov r7, sp
    mrs r8, spsr
    push {r7,r8}

    bl irq_handler

    pop {r7,r8}
    mov sp,r7
    ldmfd sp!, {r0-r12, pc}^

_undef_handler: 
    stmfd sp!, {r0-r12, lr}

    @ "UND" = 0x55 0x4E 0x44
    ldr r10, =#0x444E55

    ldmfd sp!, {r0-r12, pc}^

_prefetch_handler: 
    subs lr, lr, #8
    stmfd sp!, {r0-r12, lr}

    @ "PRE" = 0x50 0x52 0x45
    ldr r10, =#0x455250

    ldmfd sp!, {r0-r12, pc}^

_data_abort_handler:
    subs lr, lr, #8
    stmfd sp!, {r0-r12, lr}

    @ "ABT" = 0x41 0x42 0x54
    ldr r10, =#0x544241

    ldmfd sp!, {r0-r12, pc}^
.end
