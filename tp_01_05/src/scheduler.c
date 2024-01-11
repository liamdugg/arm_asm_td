#include "../inc/gic.h"
#include "../inc/mmu.h"
#include "../inc/timer.h"
#include "../inc/tasks.h"
#include "../inc/scheduler.h"

// MAPA DE MEMORIA DE UNA TAREA
//  | SECTION | OFFSET |
//  |=========|========|
//  | code    |  0x0   | -> 8K
//  | stack   | 0x2000 | -> 8K
//  | tablas  | 0x4000 | -> 36K
//  | bss     | 0xD000 | -> 4k  no utilizado dejo el espacio para implementar luego
//  | data    | 0xE000 | -> 4k  no utilizado dejo el espacio para implementar luego
//  | rodata  | 0xF000 | -> 4k  no utilizado dejo el espacio para implementar luego  
//                       
// Tamano de tarea total -> 64k = 0x10000

// TAREA IDLE -> code 7100_0000 | stack 7100_2000 | tablas de kernel
// TAREA 1    -> code 7101_0000 | stack 7101_2000 | tablas 7101_4000 | ttl2 7101_8000
// TAREA 2    -> code 7102_0000 | stack 7102_2000 | tablas 7102_4000 | ttl2 7102_8000
// Por ahora tenemos un numero estatico de tareas

// VARIABLES GLOBALES
uint32_t num_tasks;
uint32_t running_task  =0;
uint32_t previous_task =0;
uint32_t timer_ticks = IDLE_TICKS-1;
task_t scheduler[MAX_TASKS];

extern int ttl2_cant;

/* ----- TASK CREATION/DELETION ----- */

__attribute__((section(".kernel"))) uint32_t task_create(uint32_t rom_addr,uint32_t task_size ,task_priority_t priority, uint32_t ticks){

    int i = 0;

    while(i<MAX_TASKS){

        if(scheduler[i].state == DEAD){ // busco un hueco libre en el scheduler
            
            // configuro la struct de la tarea
            scheduler[i].tsk_id     = i;
            scheduler[i].state      = READY;
            scheduler[i].ticks      = ticks;
            scheduler[i].priority   = priority;                             // no hace nada, implementar despues
            
            scheduler[i].task_base  = sch_get_task_base(i);                 // ubicacion en RAM de la tarea

            // cargo los stack pointers
            scheduler[i].sp_fiq = TASK_STACK + ((uint32_t)&_STACK_SIZE)*1;          // 0x80002000 + 0x0400      
            scheduler[i].sp_irq = TASK_STACK + ((uint32_t)&_STACK_SIZE)*2 -15*4;    // 0x80002000 + 0x0800 
            scheduler[i].sp_svc = TASK_STACK + ((uint32_t)&_STACK_SIZE)*3;          // 0x80002000 + 0x0C00 
            scheduler[i].sp_abt = TASK_STACK + ((uint32_t)&_STACK_SIZE)*4;          // 0x80002000 + 0x1000 
            scheduler[i].sp_und = TASK_STACK + ((uint32_t)&_STACK_SIZE)*5;          // 0x80002000 + 0x1400 
            scheduler[i].sp_usr = TASK_STACK + ((uint32_t)&_STACK_SIZE)*6;          // 0x80002000 + 0x1800 
        
            task_to_ram(rom_addr,scheduler[i].task_base, task_size);        // copio la tarea de rom a ram                         
            task_preload_stack(i);                                          // cargo un LR de arranque en SP de IRQ
            
            // seteo ttbr0 y pagino la tarea. Tarea idle (id 0) usa tablas del kernel
            if(scheduler[i].tsk_id == IDLE_ID)
                scheduler[i].ttbr0 = (uint32_t)&_KERNEL_TTL1_START;
            
            else scheduler[i].ttbr0 = scheduler[i].task_base + TABLES_OFFSET; 

            task_page(scheduler[i]);
            num_tasks++;
            return NO_ERROR;
        }

        else i++;
    }

    return ERROR_CREATING_TASK;
}

__attribute__((section(".kernel"))) void task_page(task_t task){

    // La funcion se encarga de paginar las tareas
    // A cada tarea se le va a paginar:
    //      - Vector Table
    //      - Area de ejecucion 0x80000000 (igual para todas)
    //      - Kernel
    //      - HW a utilizar (habria que paginar todo en realidad)
    //      - .data
    //      - .bss
    //      - Zona de lectura (ver funcion task_init)

    // A la tarea idle (task id 0) ademas se le pagina:
    //      - Zona en RAM de las otras tareas
    //      - Codigo de inicializacion
    //      - Stacks del kernel
    //      - Tablas del kernel (utiliza estas
    //      - Zona de lectura de las otras tareas

    uint32_t i = 0;
    ttl2_cant = 0;
    uint32_t task_base = task.task_base;
    AP_PLX PL = 0;

    if(task.priority == SYS_PRIORITY) 
        PL = PL1_RW;
    
    else if(task.priority == USER_PRIORITY) 
        PL = PL0_RW;

    // PRIMERO PAGINO LO QUE TODAS LAS TAREAS DEBERIAN PODER VER
    // LUEGO, CONSIDERANDO A LA TAREA IDLE COMO PARTE DEL KERNEL
    // LE PAGINO TODO LO DEMAS

    // PAGINO VECTOR TABLE
    uint32_t _VTBL_END = (uint32_t)&_VTBL_VMA_START + (uint32_t)&_VTBL_SIZE;
    
    for(i=(uint32_t)&_VTBL_VMA_START; i<_VTBL_END; i+=(uint32_t)&_PAGE_SIZE)
        mmu_new_page(i,i,XN_AE, PL, task.ttbr0);
    
    
    // Pagino la zona donde se ve ejecutando la tarea como 0x8000_0000
    for(i=task_base; i<(task_base+(uint32_t)&_TASK_SIZE); i+=(uint32_t)&_PAGE_SIZE){
        uint32_t offset = i-task_base;
        mmu_new_page((TASK_BASE+offset), i, XN_AE,PL, task.ttbr0);
    }

    // PAGINO KERNEL
    uint32_t _KERNEL_END =  (uint32_t)&_KERNEL_VMA_START + (uint32_t)&_KERNEL_SIZE;
    
    for(i=(uint32_t)&_KERNEL_VMA_START; i<_KERNEL_END; i+=(uint32_t)&_PAGE_SIZE)
        mmu_new_page(i,i,XN_AE, PL, task.ttbr0);
    
    // PAGINO HW - solo lo que voy a usar
    mmu_new_page(TIMER0_BASE, TIMER0_BASE, XN_AE, PL, task.ttbr0);
    mmu_new_page(GICC0_BASE , GICC0_BASE , XN_AE, PL, task.ttbr0);
    mmu_new_page(GICC0_BASE , GICC0_BASE , XN_AE, PL, task.ttbr0);

    // PAGINO .DATA y .BSS
    for(i=(uint32_t)&_DATA_START; i<(uint32_t)&_BSS_END; i+=(uint32_t)&_PAGE_SIZE)
        mmu_new_page(i, i, XN_AE, PL, task.ttbr0);

    // PAGINO LO QUE FALTA PARA LA TAREA IDLE
    if(task.tsk_id == 0){
        
        // PAGINO DONDE ESTAN LAS OTRAS TAREAS CARGADAS EN RAM
        uint32_t TASK_RAM_END = (uint32_t)&_TASK1_VMA + 2*(uint32_t)&_TASK_SIZE;
    
        for(i=(uint32_t)&_TASK1_VMA; i<TASK_RAM_END; i+= (uint32_t)&_PAGE_SIZE)
            mmu_new_page(i,i,XN_AE,PL, task.ttbr0);

        // PAGINO AREA DE LECTURA DE LAS OTRAS TAREAS
        for(i=T1_READ_START; i<T1_READ_END; i+=(uint32_t)&_PAGE_SIZE)
            mmu_new_page(i,i,XN_AE,PL, task.ttbr0);

        for(i=T2_READ_START; i<T2_READ_END; i+=(uint32_t)&_PAGE_SIZE)
            mmu_new_page(i,i,XN_AE,PL, task.ttbr0);
        
        // PAGINO KERNEL TTLs
        for(i=(uint32_t)&_KERNEL_TABLES_START; i<(uint32_t)&_KERNEL_TABLES_END; i+=(uint32_t)&_PAGE_SIZE)
            mmu_new_page(i,i,XN_AE, PL, task.ttbr0);
        
        // PAGINO LA ZONA DE RAM CON EL CODIGO DE STARTUP
        // STARTUP - para que no tire prefetch al habilitar paginacion y aun no termina el init
        uint32_t _STARTUP_END = (uint32_t)&_STARTUP_START + (uint32_t)&_STARTUP_SIZE;

        for(i=(uint32_t)&_STARTUP_START;i<_STARTUP_END; i+=(uint32_t)&_PAGE_SIZE)
            mmu_new_page(i,i,XN_AE, PL, task.ttbr0);

        // PAGINO STACK
        uint32_t _STACK_END = (uint32_t)&_STACK_BASE + (uint32_t)&_STACK_SIZE*6;
        
        for(i=(uint32_t)&_STACK_BASE; i<_STACK_END; i+=(uint32_t)&_PAGE_SIZE)
            mmu_new_page(i,i,XN_AE, PL, task.ttbr0);
    }

    return;
}

__attribute__((section(".kernel"))) void task_rmv(uint32_t id){

    // Elimina tarea del scheduler
    // Pongo esa posicion en estado "DEAD"

    scheduler[id].ticks = 0;
    scheduler[id].tsk_id = 0;
    scheduler[id].state = DEAD;

    scheduler[id].ttbr0 = 0;
    scheduler[id].task_base = 0;

    scheduler[id].sp_fiq = 0;        
    scheduler[id].sp_irq = 0;
    scheduler[id].sp_svc = 0;
    scheduler[id].sp_abt = 0;
    scheduler[id].sp_und = 0;
    scheduler[id].sp_usr = 0;

    if(num_tasks > 0) num_tasks--;
    return;
}

__attribute__((section(".kernel"))) void task_rmv_all(){
    
    // Elimina todas las tareas del scheduler
    for(int i=0; i<MAX_TASKS; i++)
        task_rmv(i);
    return;
}

__attribute__((section(".kernel"))) void task_to_ram(uint32_t rom_addr, uint32_t ram_addr, uint32_t task_size){

    // Copia codigo de la tarea de ROM a RAM
    uint32_t* rom = (uint32_t*)rom_addr;
    uint32_t* ram = (uint32_t*)ram_addr;
    uint32_t  index = task_size/4;

    // tiene que ser el tamano de la tarea dividido 4 para el indice
    // por eso se me estaba rompiendo
    for(int i=0; i<index; i++)
        ram[i] = rom[i];
    
    return;
}

__attribute__((section(".kernel"))) void task_preload_stack(uint32_t id){
        
    uint32_t* sp_irq = (uint32_t*)(scheduler[id].task_base + 0x2800);

    sp_irq--;
    *sp_irq = 0x80000000; // Cargo en posicion donde voy a sacar el LR la direccion donde corren las tareas
    
    sp_irq -= 14;

    // cargo el spsr del modo en el que va a correr la tarea
    if(scheduler[id].priority == USER_PRIORITY)
        *(sp_irq) = 0x10;

    else if(scheduler[id].priority == SYS_PRIORITY)
        *(sp_irq) = 0x1F;
}

/* ----- SCHEDULER HANDLING ----- */

__attribute__((section(".kernel"))) uint32_t sch_time_control(){

    // Controla el tiempo de ejecucion de cada tarea.
    // Cuando se vence el tiempo de una, retorna flag indicando hacer context switch. (Esto ultimo en ASM)
    static uint8_t first_switch = 0;
    uint32_t new_id;

    timer_ticks++;

    if(!first_switch){
        first_switch = 1;
        return 2;
    }

    if(timer_ticks >= scheduler[running_task].ticks){
        
        new_id = sch_get_next_task(); // busco siguiente tarea a correr
        
        timer_ticks  = 0; // reseto timer
        
        // actualizo globales a usar en asm
        previous_task = running_task;
        running_task = new_id;

        // actualizo estado de las tareas
        scheduler[new_id].state = RUNNING;  
        scheduler[running_task].state = READY; 

        return 1; // Hay que hacer context switch
    }
    
    return 0; // NO hay que hacer context switch
}

__attribute__((section(".kernel"))) uint32_t sch_get_next_task(void){

    // Busca la siguiente tarea disponible para correr

    int i = 0;
    uint32_t id = running_task+1;

    if(id == MAX_TASKS){
        return 0;
    }
    
    while(i<MAX_TASKS){

        if(scheduler[id].state == READY)
            return id;      

        i++;                        // veces que hice el loop
        id = (id+1) % MAX_TASKS;    // indice que miro
    }
    
    return running_task; // si no encuentra otra para correr va la misma
}

/* ----- AUXILIARY FUNCTIONS ----- */
__attribute__((section(".kernel"))) uint32_t sch_get_task_base(uint32_t id){
    
    // Devuelve direccion de ram donde se carga la tarea basado en el id.
    return ((uint32_t)&_TASK_RAM_BASE) + (uint32_t)&_TASK_SIZE*id;
}
