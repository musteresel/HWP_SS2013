
SOURCES := compileTest/main.c compileTest/motor.c


compileTest: compileTest/compileTest.hex

compileTest/compileTest.elf: LDFLAGS += -Wl,-Map=compileTest/compileTest.map
compileTest/compileTest.elf: $(SOURCES:.c=.o)


-include $(SOURCES:.c=.d)

