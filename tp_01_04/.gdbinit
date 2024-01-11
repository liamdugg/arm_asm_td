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

#break kernel.s:49
#break vector_table.s:101
#break vector_table.s:112

#break tasks.c:21
#break tasks.c:41
#break tasks.c:54
#break kernel.s:50
#break startup.s:50
break irq_handlers.c:21

#break scheduler.c:292
# vector table breaks
b* 0x80000000
#b* 0x00000004
#b* 0x00000008
#b* 0x0000000c
#b* 0x00000010
#b* 0x00000014
#b* 0x00000018
#b* 0x0000001C
