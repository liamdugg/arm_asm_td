#include "../inc/gic.h"
#include "../inc/timer.h"
#include "../inc/scheduler.h"

uint32_t irq_handler(void);

/* Resuelve que hacer con id de la interrupcion */
__attribute__((section(".kernel"))) uint32_t irq_handler(){
    
    _gicc_t* const GICC0 = (_gicc_t *) GICC0_BASE;
    _timer_t* const TIMER0 = (_timer_t*) TIMER0_BASE;
    uint32_t do_context_switch = 0;
    
    // get id
    uint32_t id = GICC0->IAR & 0x3FF;
    
    // selecciono handler con el id
    switch(id){
        
        case TIMER0_ID:
            do_context_switch = sch_time_control(); 
            GICC0->EOIR = id;           // End of interrupt. Escribo el intid. 
            TIMER0->Timer1IntClr = 1;   // "register write clears interrupt output from counter"
        break;

        default: //do_nothing();
        break;
    }

    return do_context_switch;
}
