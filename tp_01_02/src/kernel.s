.code 32

/* ----- GLOBAL VARIABLES ----- */
.global _main

/* ----- SECTION: KERNEL -----  */
.section .kernel, "a"
_main:
    mov r0, #12345
    mov r1, #0
    mov r2, #1
    mov r3, #2
    svc #0xAC
    wfi @ me quedo en bajo consumo
.end
