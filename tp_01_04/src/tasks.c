#include "../inc/mmu.h"
#include "../inc/tasks.h"
#include "../inc/scheduler.h"

extern task_t scheduler[MAX_TASKS];
extern int ttl2_cant;

/* --------------- TAREA 1 ---------------
 * escribe desde T1_START_ADDR a T1_END_ADDR
 * la palabra COPY_WORD en cada posicion
 * luego leo la posicion y verifico
 * si esta bien, paso a la siguiente
 * si no, reintento en la misma
 * --------------------------------------- */

__attribute__((section(".task1"), aligned(4), naked)) void task_1(void){

    uint32_t* ram_ptr = (uint32_t*)T1_READ_START;
    
    while(1){
        *ram_ptr = COPY_WORD;       // copio la palabra a ram
        if((*ram_ptr) == COPY_WORD) // chequeo que se copie bien
            ram_ptr++;  

        if(ram_ptr >= (uint32_t*)T1_READ_END)
            ram_ptr = (uint32_t*)T1_READ_START;            
    }
}

/* --------------- TAREA 2 ---------------
 * recorro desde T2_START_ADDR a T2_END_ADDR
 * toggleo todos los bits de cada palabra que leo.
 */

__attribute__((section(".task2"), aligned(4), naked)) void task_2(void){

    uint32_t* ram_ptr = (uint32_t*)T2_READ_START;
    
    while(1){
        
        *ram_ptr = ~(*ram_ptr);
        ram_ptr++;

        if(ram_ptr >= (uint32_t*)T2_READ_END)
            ram_ptr = (uint32_t*)T2_READ_START;
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

    task_create((uint32_t)&_TASK_IDLE_LMA,(uint32_t)&_TASK_IDLE_SIZE, SYS_PRIORITY, 1);

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