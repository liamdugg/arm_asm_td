dashboard -layout breakpoints assembly registers source variables memory
dashboard memory -output /dev/pts/1

# vector table breaks
#b* 0x00000000
b* 0x00000004
b* 0x00000008
b* 0x0000000c
b* 0x00000010
b* 0x00000014
b* 0x00000018
b* 0x0000001C
