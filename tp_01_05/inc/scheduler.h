#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

/* ----- DEFINES ----- */
#define TIMEFRAME   10     // TICKS
#define MAX_TASKS   3

#define IDLE_TICKS  8
#define IDLE_ID     0
#define USER_TICKS  1

#define NO_ERROR            1
#define ERROR_CREATING_TASK 255

#define TASK_BASE       (uint32_t)(0x80000000)
#define TASK_STACK      (uint32_t)(TASK_BASE + 0X2000) // ver bien los offsets 
#define TABLES_OFFSET   0x4000

/* ----- EXTERNS ----- */
extern uint32_t _TASK_SIZE;
extern uint32_t _PAGE_SIZE;
extern uint32_t _TASK_CODE_SIZE;

extern uint32_t _VTBL_VMA_START;
extern uint32_t _VTBL_SIZE;

extern uint32_t _KERNEL_VMA_START;
extern uint32_t _KERNEL_SIZE;

extern uint32_t _DATA_START;
extern uint32_t _BSS_END;

extern uint32_t _TASK1_VMA;
extern uint32_t _TASK1_RAM_BASE;

extern uint32_t _KERNEL_TABLES_START;
extern uint32_t _KERNEL_TABLES_END;

extern uint32_t _STARTUP_START;
extern uint32_t _STARTUP_SIZE;

extern uint32_t _STACK_BASE;
extern uint32_t _STACK_SIZE;

extern uint32_t _KERNEL_TTL1_START;

extern uint32_t _TASK_RAM_BASE;
extern uint32_t _TASK_LMA_BASE;

/* A DETERMINAR LOS NIVELES DE PRIORIDAD*/
typedef enum {
    USER_PRIORITY,
    SYS_PRIORITY,
    IDLE_PRIORITY
} task_priority_t;

typedef enum {
    READY,
    RUNNING,
    SUSPENDED,
    DEAD
} task_state_t;

typedef volatile struct __attribute__((packed,aligned(4))){

    uint32_t tsk_id;
    uint32_t ticks;
    task_state_t state;
    task_priority_t priority;
    
    uint32_t ttbr0;
    uint32_t task_base;
    
    uint32_t sp_fiq;
    uint32_t sp_svc;
    uint32_t sp_abt;
    uint32_t sp_und;
    uint32_t sp_usr; // == sp_sys 
    uint32_t sp_irq;

} task_t;

/* ---------- FUNCTION PROTOTYPES ---------- */
uint32_t sch_time_control(void);
uint32_t sch_get_next_task(void);
uint32_t sch_get_task_base(uint32_t id);

void task_rmv_all(void);
void task_rmv(uint32_t id);
void task_page(task_t task);
void task_preload_stack(uint32_t id);
void task_to_ram(uint32_t rom_addr,uint32_t ram_addr, uint32_t task_size);
uint32_t task_create(uint32_t rom_addr, uint32_t task_size, task_priority_t priority, uint32_t ticks);