.code 32

.include "asm_equ.h"

/* ----- GLOBAL FUNCTIONS ----- */
.global _main
.global _load_stacks
.global _ttbr_switch
.global _save_stacks
.global _sch_init

/* ----- EXTERN VARIABLES ----- */
.extern scheduler
.extern running_task
.extern previous_task

.equ STRUCT_SIZE, 0x30

/* ----- SECTION: KERNEL -----  */
.section .kernel, "a"
_main:
   wfi
   b _main
 
/* ---------------------------- */

@ scheduler       -> direccion base del array de structs de tareas
@ running_task    -> puntero a cual tarea se va a correr
@ previous-task   -> puntero a cual tarea estaba corriendo

@ Composicion struct de tarea
@ | Offset |  Elemento  |
@ |========|=========== |
@ |  0x00  |  tsk_id    |
@ |  0x04  |  ticks     |
@ |  0x08  |  state     |
@ |  0x0C  |  priority  |  
@ |  0x10  |  ttbr0     |
@ |  0x14  |  task_base |
@ |  0x18  |  sp_fiq    | 400
@ |  0x1C  |  sp_svc    | 800 
@ |  0x20  |  sp_abt    | c00
@ |  0x24  |  sp_und    | 1000
@ |  0x28  |  sp_usr    | 1400
@ |  0x2C  |  sp_irq    | 1800
@ |========|============|
@ sizeof(task_t) = 0x30


_sch_init:

   ldr r1, = running_task
   ldr r2, = previous_task
   
   mov r3, #1
   mov r4, #0
   str r3, [r1]
   str r4, [r2]

   movs pc, r10

@ guardo sps de tarea saliente en su struct
 _save_stacks:

   @ obtengo la base del struct de tarea que quiero
   ldr r0, = scheduler
   ldr r1, = previous_task
   ldr r2, [r1]               
   mov r3, #STRUCT_SIZE
   
   mul r2, r2, r3 
   add r0, r0, r2    @ r0 = TASK_x_BASE= TASK_0_BASE + id*TASK_SIZE
   
   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_FIQ)
   str sp, [r0,#0x18] 

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_SVC)
   str sp, [r0, #0x1C] 

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_ABT)
   str sp, [r0, #0x20]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_UND)
   str sp, [r0, #0x24]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE |MODE_SYS)
   str sp, [r0, #0x28]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE| MODE_IRQ)
   str sp, [r0, #0x2C]

   mov pc, lr

_ttbr_switch:

   @ ahora cargo el struct de la tarea a correr
   ldr r0, = scheduler
   ldr r1, = running_task
   ldr r2, [r1] 
   mov r3, #STRUCT_SIZE

   mul r2, r2, r3
   add r0, r0, r2    @ r0 = TASK_x_BASE= TASK_0_BASE + id*TASK_SIZE

   @ invalidate TLB
   mcr p15, 0, r4, c8, c6, 0   @ INSTRUCTION TLB
   mcr p15, 0, r4, c8, c5, 0   @ DATA TLB
   mcr p15, 0, r4, c8, c7, 0   @ UNIFIED TLB

   @ load TTBR0
   ldr r4, [r0, #0x10]  @ me guardo el ttbr0
   ISB
   DSB
   mcr p15, 0, r4, c2, c0, 0

   mov pc, lr

_load_stacks:
   
   @ ahora cargo el struct de la tarea a correr
   ldr r0, = scheduler
   ldr r1, = running_task
   ldr r2, [r1] 
   mov r3, #STRUCT_SIZE

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_FIQ)
   ldr sp, [r0, #0x18]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_SVC)
   ldr sp, [r0, #0x1C]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_ABT)
   ldr sp, [r0, #0x20]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_UND)
   ldr sp, [r0, #0x24]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_SYS)
   ldr sp, [r0, #0x28]

   msr cpsr_c, (IRQ_DISABLE | FIQ_DISABLE | MODE_IRQ)
   ldr sp, [r0, #0x2C]

   mov pc, lr
 .end
 