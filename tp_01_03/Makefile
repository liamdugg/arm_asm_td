# Main folder name
ROOT_NAME = ${notdir ${shell pwd}}

# Configs
CHAIN=arm-linux-gnueabihf
CFLAGS=-g -std=gnu99 -Wall -mcpu=cortex-a8 -MD -I${INC}
ASFLAGS=-g -march=armv7-a -I${INC}
LDFLAGS=-g -z max-page-size=0x01000 -T 
#--orphan-handling=discard
# Folders
OBJ=obj
BIN=bin
LST=lst
INC=inc
SRC=src

# List source files
SRC_FILES_C = ${wildcard ${SRC}/*.c}
OBJ_FILES_C = ${subst ${SRC},${OBJ},${SRC_FILES_C:.c=_c.elf}}

SRC_FILES_S = ${wildcard ${SRC}/*.s}
OBJ_FILES_S = ${subst ${SRC},${OBJ},${SRC_FILES_S:.s=_s.elf}}

# Rules
default: build

build: $(BIN)/${ROOT_NAME}.bin $(OBJ)/${ROOT_NAME}.lelf

debug:
	qemu-system-arm -machine realview-pb-a8 -no-reboot -nographic -monitor telnet:127.0.0.1:12344,server,nowait -kernel ${BIN}/${ROOT_NAME}.bin -S -s & gdb-multiarch -iex "add-auto-load-safe-path /" ${OBJ}/${ROOT_NAME}.lelf

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
	rm -rf $(LST)
	rm -f .gdb_history
	clear

help:
	@echo "make build: Generates .bin"
	@echo "make clean: Deletes folders: bin/ lst/ obj/"
	@echo "make debug: Runs Cortex-A8 emulator and gdb-multiarch"

# Generating .bin file from .elf
$(BIN)/${ROOT_NAME}.bin: $(OBJ)/${ROOT_NAME}.lelf
	$(CHAIN)-objcopy -O binary $< $@

# Generating .lelf file from *.elf
$(OBJ)/${ROOT_NAME}.lelf: ${OBJ_FILES_S} ${OBJ_FILES_C}
	@echo "\nLinkeando..\n"
	@mkdir -p obj
	@mkdir -p lst
	$(CHAIN)-ld ${LDFLAGS} mem_map.ld ${OBJ_FILES_S} ${OBJ_FILES_C} -o $@ -Map $(LST)/$(ROOT_NAME).map
	readelf -a $@ > $(basename $@).txt
	$(CHAIN)-objdump -D $@ > $(basename $@).lst
	@echo "\nLinkeo finalizado \n"

# ASM sources assembly
$(OBJ)/%_s.elf: ${SRC}/%.s
	@mkdir -p bin
	@mkdir -p obj
	@mkdir -p lst
	$(CHAIN)-as $< ${ASFLAGS} -o $@ -a > $(basename $@).lst

# C sources compiling
$(OBJ)/%_c.elf: ${SRC}/%.c
	@mkdir -p bin
	@mkdir -p obj
	@mkdir -p lst
	@echo "\nCompiling .c sources"
	$(CHAIN)-gcc ${CFLAGS} -c $< -o $@


# To take into account .h files
-include ${OBJ}/*.d