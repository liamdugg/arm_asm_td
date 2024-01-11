#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define T1_READ_START   0x70A00000
#define T1_READ_END     0x70A0FFFF

#define T2_READ_START   0x70A10000         
#define T2_READ_END     0x70A1FFFF

#define COPY_WORD       0x55AA55AA

extern uint32_t _TASK_IDLE_LMA;
extern uint32_t _TASK_IDLE_VMA;
extern uint32_t _TASK_IDLE_SIZE;

extern uint32_t _TASK1_LMA;
extern uint32_t _TASK1_VMA;
extern uint32_t _TASK1_SIZE;

extern uint32_t _TASK2_LMA;
extern uint32_t _TASK2_VMA;
extern uint32_t _TASK2_SIZE;

void task_1(void);
void task_2(void);
void task_idle(void);
void task_init(void);