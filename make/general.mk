# General variables used all over the place

AR := avr-ar
AS := avr-as
CC := avr-gcc
LD := avr-ld
CPP := avr-cpp
CXX := avr-cxx
OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump
NM := avr-nm


HEXFORMAT := ihex

CPUFLAGS := -mmcu=atmega1280

#CPPFLAGS := -DF_CPU=16000000
CPPFLAGS := -DF_CPU=8000000
CPPFLAGS += -DDEBUG_SIMAVR

CFLAGS := 
CFLAGS += -Os
#CFLAGS += -O3
#CFLAGS += -O0
#CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -ffreestanding
CFLAGS += -Wall -Wextra
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
#CFLAGS += -Wunreachable-code
CFLAGS += -Wsign-compare
CFLAGS += -std=gnu99 #-std=c99
CFLAGS += -g3 -gdwarf-2 -pg #DEGUBING!
CFLAGS += -fstack-usage
CFLAGS += -I$(SOURCE_LOCATION)/ -I$(SOURCE_LOCATION)/hwplib
CFLAGS += -I/usr/include/simavr/avr/
CFLAGS += 


LDFLAGS :=
LDFLAGS += -Wl,--relax
LDFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000

