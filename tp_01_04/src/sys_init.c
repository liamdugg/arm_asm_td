#include "../inc/gic.h"
#include "../inc/mmu.h"
#include "../inc/timer.h"
#include "../inc/tasks.h"
#include "../inc/scheduler.h"

extern void _paging_enable();
extern void _paging_init();

void sys_init(void);
void gic_init(void);
void timer_init(void);

__attribute__((section(".text"))) void sys_init(void){
  
    // HW Init  
    gic_init();
    timer_init();
    //mmu_sys_paging();
    task_init();
    sch_init();

    return;
}

__attribute__((section(".text"))) void timer_init(){

    _timer_t* const TIMER0 = (_timer_t*)TIMER0_BASE;

    /* revisar la FCLK para los ticks */
    TIMER0->Timer1Load  = 0x00010000; // Tick num = 65536 
    
    TIMER0->Timer1Ctrl  = 0x00000002; // Timer de 32-bit 
    TIMER0->Timer1Ctrl |= 0x00000040; // Timer periodico
    TIMER0->Timer1Ctrl |= 0x00000020; // Timer interrupt enable
    TIMER0->Timer1Ctrl |= 0x00000080; // Timer enable
}

__attribute__((section(".text"))) void gic_init(){

    _gicc_t* const GICC0 = (_gicc_t *) GICC0_BASE;
    _gicd_t* const GICD0 = (_gicd_t *) GICD0_BASE;

    GICC0->PMR  = 0x000000F0;           // priority mask             
    GICC0->CTLR = 0x00000001;           // habilito interface
    GICD0->CTLR = 0x00000001;           // habilito distributor
    GICD0->ISENABLER[1] |= 0x00000010;  // habilito int id 36 (timer)
    GICD0->ISENABLER[1] |= 0x00000002;  // habilito int id 33 (swi)
}
