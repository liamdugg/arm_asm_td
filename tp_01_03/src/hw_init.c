#include "../inc/gic.h"
#include "../inc/timer.h"

void hw_init(void);
void gic_init(void);
void timer_init(void);

__attribute__((section(".text"))) void hw_init(void){
    gic_init();
    timer_init();
}

__attribute__((section(".text"))) void timer_init(){

    _timer_t* const TIMER0 = ( _timer_t* )TIMER0_BASE;

    TIMER0->Timer1Load     = 0x00010000; // Período = 2^18 ticks ~= 0.25 segundos
    
    TIMER0->Timer1Ctrl     = 0x00000002; // Timer de 32-bit 
    TIMER0->Timer1Ctrl    |= 0x00000040; // Timer periodico
    TIMER0->Timer1Ctrl    |= 0x00000020; // Timer interrupt enable
    TIMER0->Timer1Ctrl    |= 0x00000080; // Timer enable
}

__attribute__((section(".text"))) void gic_init(){

    static _gicc_t* GICC0 = (_gicc_t *) GICC0_BASE;
    static _gicd_t* GICD0 = (_gicd_t *) GICD0_BASE;

    GICC0->PMR          |= 0x000000F0;  // priority mask             
    GICD0->ISENABLER[1] |= 0x00000010;  // habilito int id 36 (timer)
    GICD0->ISENABLER[1] |= 0x00000002;  // habilito int id 33 (swi)

    GICC0->CTLR |= 0x1;                 // habilito interface
    GICD0->CTLR |= 0x1;                 // habilito distributor
}
