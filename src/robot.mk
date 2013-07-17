
SOURCES := \
 kernel/main.c \
 kernel/task.c \
 kernel/semaphore.c \
 kernel/pipe.c \
 util/ringbuffer.c \
 util/property.c \
 sensor/ir.c \
 sensor/incremental.c \
 device/motor.c \
 hwplib/IO/ADC/ADC.c \
 communication.c \
 alive.c \
 irsensors.c \
 sensortelemetry.c \
 commander.c \
 translationcontrol.c



robot: robot.hex robot.lss robot.sym

robot.elf: LDFLAGS += -Wl,-Map=robot.map
robot.elf: $(SOURCES:.c=.o)


-include $(SOURCES:.c=.d)

