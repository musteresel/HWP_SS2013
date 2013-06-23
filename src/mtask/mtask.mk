
SOURCES := mtask/sys_main2.c \
	mtask/task.c \
	compileTest/motor.c \
	hwplib/communication/communication.c \
	hwplib/IO/uart/uart.c \
	hwplib/IO/ADC/ADC.c


mtask: mtask/mtask.hex mtask/mtask.lss mtask/mtask.sym

mtask/mtask.elf: LDFLAGS += -Wl,-Map=mtask/mtask.map
mtask/mtask.elf: CPPFLAGS += -I. -I./hwplib/ -I$(SOURCE_LOCATION)/hwplib/
mtask/mtask.elf: $(SOURCES:.c=.o)


-include $(SOURCES:.c=.d)

