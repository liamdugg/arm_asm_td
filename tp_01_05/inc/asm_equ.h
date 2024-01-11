// DEFINES USADOS EN ASSEMBLER
.equ MODE_USR,      0x10
.equ MODE_FIQ,      0x11
.equ MODE_IRQ,      0x12
.equ MODE_SVC,      0x13
.equ MODE_ABT,      0x17
.equ MODE_UND,      0x1B
.equ MODE_SYS,      0x1F

.equ IRQ_DISABLE,   0x80    // bit 7
.equ FIQ_DISABLE,   0x40    // bit 6

.equ IRQ_ENABLE,    0x0     // no son necesarias, nada mas
.equ FIQ_ENABLE,    0x0     // para hacer mas claro el codigo
