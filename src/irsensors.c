#include <stdint.h>
#include "kernel/task.h"
#include "util/property.h"
#include "sensor/ir.h"
#include "irsensors.h"




//-----------------------------------------------------------------------------
/** Global sensor data
 * */
IrSensorData irSensorData;




/** Task to read ir values
 * */
TASK_STATIC(irReader,1,irReaderFct,200,0);




//-----------------------------------------------------------------------------
/** Task function reading ir values. This must be relatively short because it
 * is running at a very high priority!
 * */
void irReaderFct(void)
{
	IrSensorData data;
	// Don't use the first values, they may be uninitialized!
	Task_waitCurrent(500);
	do
	{
		// Read ir values
		data.front = Ir_read(IR_FRONT);
		data.left = Ir_read(IR_LEFT);
		data.right = Ir_read(IR_RIGHT);
		// Copy to global variable so that they can be read by other tasks
		Property_copy(
				(uint8_t*)&irSensorData,(uint8_t*)&data,sizeof(IrSensorData));
		// Wait some time, allowing other task to run
		Task_waitCurrent(500);
	} while (1);
}

