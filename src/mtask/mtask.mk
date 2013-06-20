
SOURCES := mtask/sys_main2.c compileTest/motor.c


mtask: mtask/mtask.hex mtask/mtask.lss mtask/mtask.sym

mtask/mtask.elf: LDFLAGS += -Wl,-Map=mtask/mtask.map
mtask/mtask.elf: $(SOURCES:.c=.o)


-include $(SOURCES:.c=.d)

