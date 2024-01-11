#include <stddef.h>
#include <stdint.h>

#define TTL1_SIZE       0x1000 
#define TTL2_SIZE       0x400

#define TTL1_ENTRIES    4096
#define TTL2_ENTRIES    256

// Descriptor types
#define INVALID_DESCRIPTOR  0b00
#define PAGETABLE           0b01
#define SMALLPAGE           0b1

#define XN_BE               0b1
#define XN_AE               0b0

// Externs
extern uint32_t _TTL1_SIZE;
extern uint32_t _TTL2_SIZE;

/* -------------------- ENUMS -------------------- */

// AP[2:0] tipo de privilegios
typedef enum {
    NO_ACCESS,
    PLX_R,
    PL0_RW,
    PL0_R,
    PL1_RW,
    PL1_R,
} AP_PLX;

// Enum para ayudar a la configuración de los bits TEX, C y B de los descriptores
typedef enum {
    _SHAREABLE,
    _SBIT,
    _NONSHAREABLE
} PAGESHAREABLE;


/* -------------------- FUNCTION PROTOTYPES -------------------- */

void mmu_ttl1_clear(uint32_t*);
void mmu_ttl2_clear(uint32_t*);
uint32_t mmu_get_ttl2_addr(uint32_t ttbr0);

void mmu_new_page(uint32_t v_addr, uint32_t p_addr, uint32_t xn, uint32_t privilege, uint32_t ttbr0);
uint32_t mmu_set_page_privilege(uint32_t descriptor, uint32_t privilege);
