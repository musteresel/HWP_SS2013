#include <stdint.h>
#include "kernel/task.h"
#include "util/communication.h"
#include "util/property.h"
#include "sensor/ir.h"



typedef struct __IrSensorData_t
{
	dist_t front;
	dist_t left;
	dist_t right;
} IrSensorData;
static IrSensorData irSensorData;

TASK_STATIC(irReader,2,irReaderFct,200,0);
TASK_STATIC(irSender,6,irSenderFct,100,0);

void irReaderFct(void)
{
	IrSensorData data;
	do
	{
		data.front = Ir_read(IR_FRONT);
		data.left = Ir_read(IR_LEFT);
		data.right = Ir_read(IR_RIGHT);
		Property_copy(
				(uint8_t*)&irSensorData,(uint8_t*)&data,sizeof(IrSensorData));
		Task_waitCurrent(500);
	} while (1);
}


typedef struct __SensorTelemetrie_t
{
	uint8_t bumbers;
	uint16_t incremental1;
	uint16_t incremental2;
	uint16_t ultra;
	uint16_t inf1;
	uint16_t inf2;
} SensorTelemetrie;



void irSenderFct(void)
{
	do
	{
		IrSensorData data;
		Task_waitCurrent(1000);
		Property_copy(
				(uint8_t*)&data,(uint8_t*)&irSensorData,sizeof(IrSensorData));
		SensorTelemetrie telemetrie;
		telemetrie.ultra = 42;
		telemetrie.inf1 = 21;
		telemetrie.inf2 = data.right;
		Communication_writePacket(1,(uint8_t *)&telemetrie,sizeof(SensorTelemetrie));
	} while (1);
}


