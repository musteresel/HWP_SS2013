
SOURCES := mtask/sys_main3.c \
	mtask/task.c


mtask: mtask/mtask.hex mtask/mtask.lss mtask/mtask.sym

mtask/mtask.elf: LDFLAGS += -Wl,-Map=mtask/mtask.map
mtask/mtask.elf: CPPFLAGS += -I. -I./hwplib/ -I$(SOURCE_LOCATION)/hwplib/
mtask/mtask.elf: $(SOURCES:.c=.o)


-include $(SOURCES:.c=.d)

