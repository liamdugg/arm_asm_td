#include "../inc/mmu.h"
#include "../inc/tasks.h"
#include "../inc/scheduler.h"

extern task_t scheduler[MAX_TASKS];
extern int ttl2_cant;

// El ejercicio pide correr tareas 1 y 2 en modo USER
// Ademas, cuando quiera leer la zona de memoria asignada
// Tengo que hacer un SVC que hace que las tareas puedan correr en modo supervisor

/* --------------- TAREA 1 ---------------
 * escribe desde T1_START_ADDR a T1_END_ADDR
 * la palabra COPY_WORD en cada posicion
 * luego leo la posicion y verifico
 * si esta bien, paso a la siguiente
 * si no, reintento en la misma
 * --------------------------------------- */

__attribute__((section(".task1"), aligned(4), naked)) void task_1(void){

    uint32_t* ram_ptr = (uint32_t*)T1_READ_START;
    
    // si saco esta linea salto a data abort al tratar de leer en ram
    asm("svc #0xA1");   // pido que me deje leer pasandole la direccion

    while(1){

        asm("mov r9, #0x1111");     // para debuguear

        *ram_ptr = COPY_WORD;       // copio la palabra a ram
        if((*ram_ptr) == COPY_WORD) // chequeo que se copie bien
            ram_ptr++;  

        if(ram_ptr >= (uint32_t*)T1_READ_END){
            ram_ptr = (uint32_t*)T1_READ_START;   
            asm("wfi"); // para debuguear
        }        
    }
}

/* --------------- TAREA 2 ---------------
 * recorro desde T2_START_ADDR a T2_END_ADDR
 * toggleo todos los bits de cada palabra que leo.
 */

__attribute__((section(".task2"), aligned(4), naked)) void task_2(void){

    uint32_t* ram_ptr = (uint32_t*)T2_READ_START;
    
    // si saco esta linea salto a data abort al tratar de leer en ram
    asm("svc #0xA2"); // pido que me deje leer pasandole la direccion
    
    while(1){
        
        asm("mov r9, #0x2222"); // para debuguear

        *ram_ptr = ~(*ram_ptr);
        ram_ptr++;

        if(ram_ptr >= (uint32_t*)T2_READ_END){
            ram_ptr = (uint32_t*)T2_READ_START;
        }
        
    }
}

/* --------------- TAREA IDLE --------------- */
__attribute__((section(".task_idle"), aligned(4), naked)) void task_idle(void){

    while(1){
        asm("wfi");
    }
    return;
}

__attribute__((section(".startup"), aligned(4))) void task_init(void){

    task_rmv_all();

    // La paginacion de las zonas de memoria a leer estan en PL1 para lectura y escritura.
    // Las tareas no van a poder acceder a esas zonas ya que van a estar en modo user (PL0).
    
    task_create((uint32_t)&_TASK_IDLE_LMA,(uint32_t)&_TASK_IDLE_SIZE, SYS_PRIORITY, 8);

    task_create((uint32_t)&_TASK1_LMA, (uint32_t)&_TASK1_SIZE, USER_PRIORITY, 1);
        
    // pagino area de lectura de T1
    for(int i=T1_READ_START; i<T1_READ_END; i+=(uint32_t)&_PAGE_SIZE)
        mmu_new_page(i,i,XN_AE, PL1_RW, scheduler[1].ttbr0);

    task_create((uint32_t)&_TASK2_LMA, (uint32_t)&_TASK2_SIZE, USER_PRIORITY, 1);

    // pagino area de lectura de T2
    for(int i=T2_READ_START; i<T2_READ_END; i+=(uint32_t)&_PAGE_SIZE)
        mmu_new_page(i,i,XN_AE, PL1_RW, scheduler[2].ttbr0);
    
    return;
}