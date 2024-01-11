#include <stddef.h>
#include <stdint.h>

#define TTL1_SIZE       0x1000 
#define TTL2_SIZE       0x400

#define TTL1_ENTRIES    4096
#define TTL2_ENTRIES    256

// Descriptor types
#define INVALID_DESCRIPTOR  0b00
#define PAGETABLE           0b01
#define RESERVED            0b11
#define SECTION             0b1
#define LARGEPAGE           0b01
#define SMALLPAGE           0b1

// PAGE TABLES defines
#define NS_SECURE           0b1
#define NS_NONSECURE        0b0

#define XN_BE               0b1
#define XN_AE               0b0

#define S_SHAREABLE         0b1
#define S_NONSHAREABLE      0b0

#define NG_GLOBAL           0b0
#define NG_NOTGLOBAL        0b1

// Externs
extern uint32_t _TTL1_SIZE;
extern uint32_t _TTL2_SIZE;

/* -------------------- ENUMS -------------------- */

// DACR Register
typedef enum {
    DOMAIN_NO_ACCESS = 0b00,
    DOMAIN_CLIENT = 0b01,
    DOMAIN_RESERVED = 0b10,
    DOMAIN_MANAGER = 0b11
} DOMAIN_AP;

// AP[2:0] tipo de privilegios
typedef enum {
    NO_ACCESS,
    PLX_R,
    PL0_RW,
    PL0_R,
    PL1_RW,
    PL1_R,
} AP_PLX;

// Access Flag, usa o no usa
// BORRABLE
 typedef enum {
    AF_YES = 0b1,
    AF_NO = 0b0
} ENABLEAF;

// TEX remap, si o no
// BORRABLE 
typedef enum {
    TEXREMAP_YES = 0b1,
    TEXREMAP_NO = 0b0
} ENABLETEXREMAP;

// Determinar si se usan las Data and instruction barrier operations
// BORRABLE
typedef enum {
    CP15BEN_YES = 0b1,
    CP15BEN_NO = 0b0
} ENABLECP15BEN;

// Determina si se usa la MMU
typedef enum {
    MMUENABLE_YES = 0b1,
    MMUENABLE_NO = 0b0
} MMUENABLE;

// ayudar a la configuración de los descriptores
// BORRABLE
typedef enum {
    FIRSTLEVEL,
    SECONDLEVEL
} TTLEVEL;

// Tamaño de una página
// BORRABLE SOLO USAMOS DE 4K
typedef enum {
    _4K,
    _64K,
    _1M
} PAGESIZE;

// Enum para ayudar a la configuración de los bits TEX, C y B de los descriptores
typedef enum {
    _STRONGLYORDERED,
    _DEVICE,
    _NORMAL
} MEMORYTYPE;

// Enum para ayudar a la configuración de los bits TEX, C y B de los descriptores
typedef enum {
    _SHAREABLE,
    _SBIT,
    _NONSHAREABLE
} PAGESHAREABLE;

// Enum para ayudar a la configuración de los bits TEX, C y B de los descriptores
typedef enum {
    STRONGLYORDERED,
    SHAREABLE,
    WRITETHROUGH,
    WRITEBACK,
    NONCACHEABLE,
    NONSHAREABLE,
    CACHEABLEMEMORY
} DESCRIPTION;

// Enum para ayudar a la configuración de los bits TEX, C y B de los descriptores
typedef enum {
    WA_YES,
    WA_NO,
    WA_NA
} WRITEALLOC;

/* --------------- REGISTERS TYPEDEFS --------------- */

// TTBCR
typedef union {
    uint32_t ttbcr;
    struct {
        uint32_t T0SZ       :3;
        uint32_t RESERVED0  :1;
        uint32_t PD0        :1;
        uint32_t PD1        :1;
        uint32_t RESERVED1  :1;
        uint32_t EPD0       :1;
        uint32_t IRGN0      :2;
        uint32_t ORGN0      :2;
        uint32_t SH0        :2;
        uint32_t RESERVED2  :2;
        uint32_t T1SZ       :3;
        uint32_t RESERVED3  :3;
        uint32_t A1         :1;
        uint32_t EPD1       :1;
        uint32_t IRGN1      :2;
        uint32_t ORGN1      :2;
        uint32_t SH1        :2;
        uint32_t IDF        :1;
        uint32_t EAE        :1;
    };
} TTBCR;

// TTBR0
typedef union{
    uint32_t ttbr0;
    struct{
        uint32_t IRGN_1 : 1;
        uint32_t S      : 1;
        uint32_t IMP    : 1;
        uint32_t RGN    : 2;
        uint32_t NOS    : 1;
        uint32_t IRGN2  : 1;
        uint32_t TTB0A  : 25;    
    };
} TTBR0;

// TTBR1
typedef union{
    uint32_t ttbr1;
    struct{
        uint32_t IRGN_1    :  1;
        uint32_t S         :  1;
        uint32_t IMP       :  1;
        uint32_t RGN       :  2;
        uint32_t NOS       :  1;
        uint32_t IRGN2     :  1;
        uint32_t RESERVED0 :  7;
        uint32_t TTB0A     : 18;  
    };
} TTBR1;

// L1 - PAGE TABLE DESCRIPTOR
typedef union {
    uint32_t pt_desc;
    struct {
        uint32_t DESC_TYPE              :  2;
        uint32_t PXN                    :  1;
        uint32_t NS                     :  1;
        uint32_t SBZ                    :  1;
        uint32_t DOMAIN                 :  4;
        uint32_t IMP_DEFINED            :  1;
        uint32_t PAGETABLE_BASE_ADDRESS : 22;
    };
} PAGETABLE_DESCRIPTOR;

// L2 - SMALL PAGE DESCRIPTOR
typedef union {
    uint32_t sp_desc;
    struct{
        uint32_t XN           :  1;
        uint32_t DESC_TYPE    :  1;
        uint32_t B            :  1;
        uint32_t C            :  1;
        uint32_t AP_10        :  2;
        uint32_t TEX          :  3;
        uint32_t AP_2         :  1;
        uint32_t S            :  1;
        uint32_t NG           :  1;
        uint32_t SPAGE_BASE_ADDR : 20;
    };
} SMALL_PAGE_DESCRIPTOR;

// SYSTEM CONTROL REGISTER VMSA
typedef union {
    uint32_t sctlr;
    struct {
        uint32_t M         : 1;
        uint32_t A         : 1;
        uint32_t C         : 1; // Cache Enable
        uint32_t RESERVED0 : 2;
        uint32_t CP15BEN   : 1;
        uint32_t RESERVED1 : 1;
        uint32_t B         : 1;
        uint32_t RESERVED2 : 2;
        uint32_t SW        : 1;
        uint32_t Z         : 1; // Branch Prediction Enable
        uint32_t I         : 1; // Instruction Cache Enable
        uint32_t V         : 1;
        uint32_t RR        : 1;
        uint32_t RESERVED3 : 2;
        uint32_t HA        : 1;
        uint32_t RESERVED4 : 1;
        uint32_t WXN       : 1;
        uint32_t UWXN      : 1;
        uint32_t FI        : 1;
        uint32_t U         : 1;
        uint32_t RESERVED5 : 1;
        uint32_t VE        : 1;
        uint32_t EE        : 1;
        uint32_t RESERVED6 : 1;
        uint32_t NMFI      : 1;
        uint32_t TRE       : 1;
        uint32_t AFE       : 1;
        uint32_t TE        : 1;
        uint32_t RESERVED7 : 1;
    };
} SCTLR;

// DOMAIN ACCESS CONTROL REGISTER VMSA
typedef union {
    uint32_t dacr;
    struct{
        uint32_t d0  : 2;
        uint32_t d1  : 2;
        uint32_t d2  : 2;
        uint32_t d3  : 2;
        uint32_t d4  : 2;
        uint32_t d5  : 2;
        uint32_t d6  : 2;
        uint32_t d7  : 2;
        uint32_t d8  : 2;
        uint32_t d9  : 2;
        uint32_t d10 : 2;
        uint32_t d11 : 2;
        uint32_t d12 : 2;
        uint32_t d13 : 2;
        uint32_t d14 : 2;
        uint32_t d15 : 2;
    };
} DACR;

/* -------------------- FUNCTION PROTOTYPES -------------------- */
TTBR0    mmu_get_ttbr0();
TTBCR    mmu_get_ttbcr();
uint32_t mmu_get_ttl1_addr();
uint32_t mmu_get_ttl2_addr(uint32_t ttbr0);
void mmu_sys_paging();

void mmu_set_ttbr0(uint32_t);
void mmu_new_page(uint32_t v_addr, uint32_t p_addr, uint32_t xn, uint32_t privilege, uint32_t ttbr0);
void mmu_ttl1_clear(uint32_t*);
void mmu_ttl2_clear(uint32_t*);
uint32_t mmu_set_page_privilege(uint32_t descriptor, uint32_t privilege);
