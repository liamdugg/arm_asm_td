.code 32

@ --------------- VIRTUAL MEMORY ADDRESS ---------------
@
@   31              20 19              12 11         0
@  |  L1 TABLE INDEX  |  L2 TABLE INDEX  | PAGE INDEX |

@ Tenemos 12 bits para indexar en L1. 
@ Puedo apuntar a 4096 elementos de 4 bytes c/u.
@ Se deduce que una pagina L1 ocupa en memoria 16KB.

@ Tenemos 8 bits para indexar en L2. 
@ Puedo apuntar a 256 elementos (0-255) de 4 bytes c/u.
@ Se deduce que una pagina L2 ocupa en memoria 1KB.

@ Tenemos 12 bits para indexar la pagina
@ Puedo apuntar a 4096 elementos (0-4095) de 4 bytes c/u.
@ Como las instrucciones son de 32 bits, puedo meter 4096.
@ Pagina de 4kbits

@ --------------- PB-A8 mem mapping ---------------
@ La PB-A8 tiene 256 MB
@ Para poder direccionar esa cantidad vamos a tener:
@ 64000 paginas de 4K
@   256 tablas  de L2
@     1 tablas  de L1

@ Necesito 256k + 16k = 272k de memoria para las tablas
@ En 0x0 tengo la vector table
@ Puedo poner las tablas inmediatamente despues? NO!! Hay que alinear
@ Despues de alinear ya puedo guardar las tablas

@ --------------- TRANSLATION TABLE BASE REGISTER ---------------
@ TTBR0: 
@      [0]     0   Cachable
@      [1]     0   Shareable
@      [2]     X   Implementation defined
@      [4:3]   00  Region -> Normal memory, outer non cacheable (evita problemas del qemu con cache)
@      [5]     X   Not outer shareable -> ignored no shareable page
@      [13:16] X   rsrv, SBZP
@      [31:14] Translation table base 0 address 

@ En TTBRx guardo base address de la tabla L1 de [31:14].
@ La direccion mas chica es 0x00004000 (16k) ya que en 0x0 tengo la vector table.
@ como necesito 272k para todas las tablas voy a reservar de 16k a 16k+272k para esto.

.extern mmu_identity_paging
.extern _KERNEL_TTL1_START

.global _paging_init
.global _paging_enable

.section .kernel, "a"
_paging_init:

    @ seteo domains con DACR
    ldr r0, =0xFFFFFFFF
    ISB
    DSB
    mcr p15, 0, r0, c3, c0, 0   @ Write DACR

    @ seteo TTBCR
    mrc p15, 0, r0, c2, c0, 2   @ Read TTBCR
    bic r0, r0, #0x7
    ISB
    DSB
    mcr p15, 0, r0, c2, c0, 2

    @ seteo SCTLR
    mrc p15, 0, r0, c1, c0, 0   @ Read SCTRL
    ldr r1, =0x30001804
    bic r0, r0, r1              @ C=0, Z=0, I=0, TRE =0, AFE=0
    ISB
    DSB
    mcr p15, 0, r0, c1, c0, 0   @ Write SCTRL

    @ Cargo TTBR0
    ldr r0, =_KERNEL_TTL1_START
    mcr p15, 0, r0, c2, c0, 0 

    movs pc, lr

_paging_enable:

    @ invalidate TLB
    mcr p15, 0, r0, c8, c6, 0   // INSTRUCTION TLB
    mcr p15, 0, r0, c8, c5, 0   // DATA TLB
    mcr p15, 0, r0, c8, c7, 0   // UNIFIED TLB

    @ activamos la mmu
    mrc p15, 0, r0, c1, c0, 0   @ Read SCTRL
    orr r0, #0x1          
    ISB
    DSB        
    mcr p15, 0, r0, c1, c0, 0   @ Write SCTRL
    DSB

    @ si no pongo las instrucciones de ISB e ISB
    @ la irq me devuelve id 1023 y se cuelga el programa.

    @ invalidate TLB (no se si es necesario esta segunda vez)
    mcr p15, 0, r0, c8, c5, 0   // DATA TLB
    mcr p15, 0, r0, c8, c6, 0   // INSTRUCTION TLB
    mcr p15, 0, r0, c8, c7, 0   // UNIFIED TLB

    movs pc, lr


.end
