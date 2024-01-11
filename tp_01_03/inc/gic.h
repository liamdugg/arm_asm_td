#include <stddef.h>
#include <stdint.h>

#define reserved_bits(x,y,z) uint8_t reserved##x[ z - y + 1 ];

#define GICC0_BASE      0x1E000000
#define GICD0_BASE      0x1E001000

#define TIMER0_ID       36

typedef volatile struct {

    uint32_t CTLR;  // 0x0
    uint32_t PMR;
    uint32_t BPR;
    uint32_t IAR;
    uint32_t EOIR;
    uint32_t RPR;
    uint32_t HPPIR; // 0x18
} _gicc_t;

typedef volatile struct {

    uint32_t CTLR;
    uint32_t TYPER;
    reserved_bits(0,0x0008, 0x00FC);
    uint32_t ISENABLER[3];
    reserved_bits(1,0x010C, 0x017C);
    uint32_t ICENABLER[3];
    reserved_bits(2,0x018C, 0x01FC);
    uint32_t ISPENDR[3];
    reserved_bits(3,0x020C, 0x027C);
    uint32_t ICPENDR[3];
    reserved_bits(4,0x028C, 0x02FC);
    uint32_t ISACTIVER[3];
    reserved_bits(5,0x030C, 0x03FC);
    uint32_t IPRIORITYR[24];
    reserved_bits(6,0x0460, 0x07FC);
    uint32_t ITARGETSR[24];
    reserved_bits(7,0x0860, 0x0BFC);
    uint32_t ICFGR[6];
    reserved_bits(8,0x0C18, 0x0EFC);
    uint32_t SGIR;
    reserved_bits(9,0x0F04, 0x0FFC);
} _gicd_t;