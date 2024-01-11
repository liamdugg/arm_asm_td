set radix 16
dashboard -layout breakpoints assembly registers source variables memory expression
dashboard memory -output /dev/pts/1
dashboard expression -output /dev/pts/1

dashboard memory watch 0x0
dashboard memory watch 0x80000000
dashboard memory watch 0x80002000
dashboard memory watch 0x80004000

# Kernel mem watch
#dashboard memory watch 0x70040000
#dashboard memory watch 0x70044000

# Task 1 mem watch
#dashboard memory watch 0x71014000         
#dashboard memory watch 0x71014000+0x4000

# Task 2 mem watch
#dashboard memory watch 0x71024000
#dashboard memory watch 0x71024000+0x4000

dashboard memory watch 0x70A00000
dashboard memory watch 0x70A10000

#dashboard memory watch 0x80002800-0x40 64

dashboard expression watch ((long*)running_task)
dashboard expression watch ((long*)previous_task)
dashboard expression watch ((long*)timer_ticks)

break vector_table.s:84
# vector table breaks

# RESET b* 0x00000000
# UNDEFINED b* 0x00000004
# SVC b* 0x00000008
# DATA ABT b* 0x0000000c
# PREFETCH
b* 0x00000010
# IRQ
b* 0x00000018
# FIQ b* 0x0000001C
