
SOURCES := mtask/sys_main3.c \
	kernel/task.c \
	kernel/semaphore.c
#	sensor/ir.c \
	hwplib/IO/ADC/ADC.c \
	hwplib/communication/communication.c \
	hwplib/IO/uart/uart.c


mtask: mtask/mtask.hex mtask/mtask.lss mtask/mtask.sym

mtask/mtask.elf: LDFLAGS += -Wl,-Map=mtask/mtask.map
mtask/mtask.elf: CPPFLAGS +=  
mtask/mtask.elf: $(SOURCES:.c=.o)


-include $(SOURCES:.c=.d)

