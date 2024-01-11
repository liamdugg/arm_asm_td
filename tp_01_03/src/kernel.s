.code 32

/* ----- GLOBAL VARIABLES ----- */
.global _main

/* ----- SECTION: KERNEL -----  */
.section .kernel, "a"
_main:
   wfi
   b _main
.end
