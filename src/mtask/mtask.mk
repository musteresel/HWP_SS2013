
SOURCES := mtask/sys_main3.c \
	kernel/task.c \
	sensor/ir.c \
	hwplib/IO/ADC/ADC.c \
	hwplib/communication/communication.c \
	hwplib/IO/uart/uart.c


mtask: mtask/mtask.hex mtask/mtask.lss mtask/mtask.sym

mtask/mtask.elf: LDFLAGS += -Wl,-Map=mtask/mtask.map
mtask/mtask.elf: CPPFLAGS +=  -I$(SOURCE_LOCATION)/ -I$(SOURCE_LOCATION)/hwplib
mtask/mtask.elf: $(SOURCES:.c=.o)


-include $(SOURCES:.c=.d)

