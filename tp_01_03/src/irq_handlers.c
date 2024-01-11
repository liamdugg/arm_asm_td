#include "../inc/gic.h"
#include "../inc/timer.h"

void irq_handler(void);

/* Retorna id de la fuente de interrupcion */
__attribute__((section(".kernel"))) void irq_handler(){
    
    _gicc_t*  const GICC0  = (_gicc_t *)  GICC0_BASE;
    _timer_t* const TIMER0 = (_timer_t *) TIMER0_BASE;
    
    uint32_t id = (GICC0->IAR) & (0x3FF); /* read int id*/

    /* selecciono handler con el id */
    switch(id){

        case TIMER0_ID: 
            GICC0->EOIR = id;           // End of interrupt. Escribo el int id. 
            TIMER0->Timer1IntClr = 1;   // Writing this reg clears interrupt output from counter"
        break;

        default: break; // ignoro otros int ids
    }
}
