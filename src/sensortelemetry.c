#include <stdint.h>
#include "kernel/task.h"
#include "util/property.h"
#include "communication.h"
#include "irsensors.h"




//-----------------------------------------------------------------------------
/** Telemetrie packet format, must be send on channel 1
 * */
typedef struct __SensorTelemetrie_t
{
	uint8_t bumbers;
	uint16_t incremental1;
	uint16_t incremental2;
	uint16_t ultra;
	uint16_t inf1;
	uint16_t inf2;
} SensorTelemetrie;




//-----------------------------------------------------------------------------
/** Task sending telemetry data
 * */
TASK_STATIC(telemetrieSender,6,telemetrieSenderFct,100,0);




//-----------------------------------------------------------------------------
/** Actual sending function, reads values from all available sensor, and
 * writes a packet to the transmitter.
 * */
void telemetrieSenderFct(void)
{
	IrSensorData ir;
	do
	{
		Task_waitCurrent(1000);
		Property_copy(
				(uint8_t*)&ir,(uint8_t*)&irSensorData,sizeof(IrSensorData));
		SensorTelemetrie telemetrie;
		telemetrie.ultra = ir.front;
		telemetrie.inf1 = ir.left;
		telemetrie.inf2 = ir.right;
		Communication_writePacket(1,(uint8_t *)&telemetrie,sizeof(SensorTelemetrie));
	} while (1);
}

