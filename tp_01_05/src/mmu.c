#include "../inc/gic.h"
#include "../inc/mmu.h"
#include "../inc/timer.h"
#include "../inc/tasks.h"

int ttl2_cant = 0;

__attribute__((section(".text"))) void mmu_new_page(
    uint32_t v_addr, 
    uint32_t p_addr, 
    uint32_t xn, 
    uint32_t privilege,
    uint32_t ttbr0){

    p_addr &= 0xFFFFF000; 

    uint32_t l1_offset = (v_addr >> 20) & 0xFFF;
    uint32_t l2_offset = (v_addr >> 12) & 0xFF;
    
    uint32_t* smallpage = NULL;
    uint32_t* pagetable = (uint32_t*)ttbr0; /* Obtengo dir tabla L1 */
    
    /* hay un descriptor de page table en ese offset? */
    /* Si no hay, obtengo dir a una tabla L2 */

    if( (pagetable[l1_offset] & 0x3) != PAGETABLE ){

        smallpage = (uint32_t*)mmu_get_ttl2_addr(ttbr0);            /* obtengo dir tabla L2 */
        pagetable[l1_offset] = ((uint32_t)smallpage & 0xFFFFFC00);  /* PXN=0,NS=0,SBZ=0,DOMAIN=0,IMP_DEFINED=0 */
        pagetable[l1_offset] |= (1<<0);                             /* seteo el descriptor type*/
    }

    // Si hay, no tengo que obtener dir de tabla L2
    else smallpage = (uint32_t*)(pagetable[l1_offset] & 0xFFFFFC00);

    // addr=p_addr,ng=0,s=0,ap2=priv,tex=0b010, 5-4 ap10,c=0,b=0,1,XN=0
    smallpage[l2_offset]  = 0;
    smallpage[l2_offset]  = p_addr;
    smallpage[l2_offset] |= (_NONSHAREABLE<<6); 
    smallpage[l2_offset] |= (SMALLPAGE <<1);
    smallpage[l2_offset] &= ~(xn<<1);
    smallpage[l2_offset]  = mmu_set_page_privilege(smallpage[l2_offset], privilege);
    
    return;
}

__attribute__((section(".text"))) uint32_t mmu_get_ttl2_addr(uint32_t ttbr0){
    
    // obtengo la direccion de una tabla L2 a partir de la tabla L1
    // a su vez, obtengo la L1 del ttbr0.

    uint32_t addr = ttbr0 + (uint32_t)&_TTL1_SIZE;

    addr += ((uint32_t)&_TTL2_SIZE) * ttl2_cant;
    mmu_ttl2_clear((uint32_t*)addr);
    
    if(ttl2_cant <= 63) ttl2_cant++;
    else ttl2_cant = 0;

    return addr;
}

/* --------------- AUX FUNCTIONS --------------- */

__attribute__((section(".text"))) uint32_t mmu_set_page_privilege(uint32_t descriptor, uint32_t privilege){

    switch (privilege){
        case PLX_R:  descriptor |= (1<<9)|(1<<4); break;

        case PL0_RW: descriptor |= (3<<4); break;

        case PL0_R:  descriptor |= (2<<4); break;

        case PL1_RW: descriptor |= (1<<4); 
                     descriptor &= ~(1<<9);
        break;

        case PL1_R:  descriptor |= (1<<9)|(1<<4); break;
            
        default: break; // NO_ACCESS
    }
    return descriptor;
}

__attribute__((section(".text"))) void mmu_ttl2_clear(uint32_t* ttl2_addr){
     for(int i=0; i < TTL2_ENTRIES; i++)
        ttl2_addr[i] = INVALID_DESCRIPTOR;
}
