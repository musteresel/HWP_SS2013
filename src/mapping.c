#include <stdint.h>
#include <math.h>
#include "kernel/task.h"
#include <avr/interrupt.h>
#include "sensor/incremental.h"
#include "communication.h"
#include "util/w1r1.h"
#include "mapping.h"
#include "pathtracking.h"
#include "sensor/ir.h"


#define WIDTH 113
#define LENGTH_TO_MID 50
#define PI 3.14159265359d



#define NORTH 0
#define NORTHEAST 1
#define EAST 2
#define SOUTHEAST 3
#define SOUTH 4
#define SOUTHWEST 5
#define WEST 6
#define NORTHWEST 7


Pose _robotPose;
W1R1 robotPose;


Cell map[MAP_SIZE][MAP_SIZE];



void updatePose(WheelDistance distance)
{
	Pose current = _robotPose;
	double sum = (distance.left + distance.right) / 2;
	double diff = distance.left - distance.right;
	double dTheta = diff / WIDTH;
	current.theta += dTheta;
	if (current.theta < 0)
	{
		current.theta += 2 * PI;
	}
	else if (current.theta > 2 * PI)
	{
		current.theta -= 2 * PI;
	}
	double sinTheta = sin(current.theta);
	double cosTheta = cos(current.theta);
  double dx = (sum * cosTheta) - (LENGTH_TO_MID * sinTheta)*dTheta;
	double dy = (sum * sinTheta) + (LENGTH_TO_MID * cosTheta)*dTheta;
	/*dx >>= 1;
	dy >>= 1;*/
	current.x += dx;
	current.y += dy;
	W1R1_write(&robotPose,&current);
}

TASK_STATIC(mapping,1,mappingFct,190,0);


// Gibt eine der oben definierten Richtungen zurueck
static uint8_t getDirection(double theta)
{
	return (((uint8_t)(theta + 22.5 * PI / 180.0) / (uint8_t)(45.0 * PI / 180.0)) & 7);
}

// Gibt die zu dir entgegengesetzte Richtung zurueck
static uint8_t getInverseDirection(uint8_t dir)
{
	return (dir + 4) & 7;
}

// Legt bei der angegebenen Zelle fest, dass man sich in direction bewegen kann
static void setCellDir(Cell * c, uint8_t direction)
{
	c->directions |= 1 << direction;
}

uint8_t getNextCell(
		int8_t x, int8_t y, uint8_t direction, int8_t * nextX, int8_t * nextY)
{

	if (direction == NORTHEAST || direction == NORTH || direction == NORTHWEST)
	{
		*nextY = y - 1;
	}
	else if (direction != WEST && direction != EAST)
	{
		*nextY = y + 1;
	}
	if (direction == NORTHEAST || direction == EAST || direction == SOUTHEAST)
	{
		*nextX = x + 1;
	}
	else if (direction != NORTH && direction != SOUTH)
	{
		*nextX = x - 1;
	}
	if (*nextX >= 0 && *nextY >= 0 && *nextX < MAP_SIZE && *nextY < MAP_SIZE)
	{
		return 1;
	}
	return 0;
}





// Aktualisiert die Zellen von der aktuellen Pose aus
// in die angegebene Richtung des IR-Sensors
static void updateCellRow(dist_t length, double theta)
{
	uint8_t x = (_robotPose.x + MAP_SIZE/2*CELL_SIZE) 
		/ CELL_SIZE;	// Zellenindex in x-Richtung
	uint8_t y = (_robotPose.y + MAP_SIZE/2*CELL_SIZE) 
		/ CELL_SIZE;	// Zellenindex in y-Richtung

	uint8_t dir = getDirection(theta);

	Cell start = map[x][y];

	while (length > CELL_SIZE)
	{
		int8_t nextX, nextY;
		if (!getNextCell(x,y,dir,&nextX, &nextY))
		{
			break;
		}
		x = nextX;
		y = nextY;
		Cell nextCell = map[x][y];
		setCellDir(&start,dir);
		setCellDir(&nextCell,getInverseDirection(dir));
		length -= CELL_SIZE;
		start = nextCell;
	}
}



// Aktualisiert die Richtungsinformationen der Zellen der Karte abhaengig von der aktuellen Pose und den Infrarotsensoren
static void updateCells(void)
{
	dist_t left, right, front;
	left = Ir_read(IR_LEFT);
	right = Ir_read(IR_RIGHT);
	front = Ir_read(IR_FRONT);
	updateCellRow(left, _robotPose.theta - PI);		// Links
	updateCellRow(right, _robotPose.theta + PI);		// Rechts
	updateCellRow(front, _robotPose.theta);			// Geradeaus
}


static void mapInit(void)
{
	uint8_t x;
	uint8_t y;
	uint16_t dist = 0;
	uint8_t halfMap = MAP_SIZE >> 1;
	for (x = 0; x < MAP_SIZE; x++)
	{
		for (y = 0; y < MAP_SIZE; y++)
		{
			map[x][y].directions = 0;	//Alle Bits auf 0 setzen
			// Quadratischen Abstand von der Kartenmitte berechnen
			dist = (x - halfMap)*(x - halfMap) + (y - halfMap)*(y - halfMap);
			// Parabelfoermige Funktion zum Umrechnen in Potential
			map[x][y].pot = -127 * dist*dist / (halfMap*halfMap*halfMap*halfMap << 2);
		}
	}
}





static void mappingFct(void)
{
	_robotPose.x = 0;
	_robotPose.y = 0;
	_robotPose.theta = 0;
	W1R1_init(&robotPose, &_robotPose, sizeof(Pose));
	mapInit();
	do
	{
		// Lese inc aus
		WheelDistance distance = Incremental_getDistance();
		// Berechne neue Pose
		updatePose(distance);
		// Lese ir aus && Berechne Wände
		updateCells();
		// (TODO)evtl: Korrigiere Pose
		// Warte 200ms
		Task_waitCurrent(200);
	} while (1);
}


//-----------------------------------------------------------------------------
TASK_STATIC(mappingSend,4,send,200,1);
struct PosePacket
{
	int32_t x;
	int32_t y;
	int16_t theta;
};

static void send(void)
{
	Task_waitCurrent(500);
	do
	{
		struct PosePacket current;
		Pose pose;
		W1R1_read(&robotPose,&pose);
		current.x = pose.x;
		current.y = pose.y;
		current.theta = (pose.theta * 180) / PI;
		current.theta -= 270;
		if (current.theta < 0)
		{
			current.theta += 360;
		}
		Communication_writePacket(2,(uint8_t*)&current,sizeof(struct PosePacket));
		Task_waitCurrent(2000);
	} while(1);
}
//-------------------------------------------------------------------------------




// Stack und Prioritaet sind ausgedacht, muss vielleicht geaendert werden
TASK_STATIC(KI,3,think,50,0);

static void think(void)
{
	uint8_t x = 0, xSend = 0;
	uint8_t y = 0, ySend = 0;
	int8_t bestPot = 127;

	do
	{
		Semaphore_wait(&waypointFlag);
		// Convert position to map position
		x = (_robotPose.x + MAP_SIZE/2*CELL_SIZE) / CELL_SIZE;
		y = (_robotPose.y + MAP_SIZE/2*CELL_SIZE) / CELL_SIZE;
		// Get cell at current map position
		Cell currentCell = map[x][y];
		// Find best neighbour cell
		xSend = x;
		ySend = y;
		uint8_t i;
		for (i = 0; i < 8; i++)
		{
			/*
			x_ = x;
			y_ = y;*/

			if (currentCell.directions & (1 << i))
			{
				int8_t neighbourX, neighbourY;
				if (getNextCell(x,y,i,&neighbourX,&neighbourY))
				{
					if (map[neighbourX][neighbourY].pot < bestPot)
					{
						bestPot = map[neighbourX][neighbourY].pot;
						xSend = neighbourX;
						ySend = neighbourY;
					}
				}
				else	// Freiheit
				{
					// Wegpunkt draussen
					// --- DANIEL.TASK_BEENDEN(); ---
					while (1)
					{
						Task_waitCurrent(65000);
					}
				}
			}
		}
		// Drive backwards in case no cell has been found 
		Waypoint wp;
		if (xSend == x && ySend == y)	// Keine Zelle gefunden
		{
			// Theta --> direction
			// GetnextCell ohne pruefung
		}
		else
		{
			wp.x = xSend*CELL_SIZE + CELL_SIZE/2 - MAP_SIZE/2*CELL_SIZE;
			wp.y = ySend*CELL_SIZE + CELL_SIZE/2 - MAP_SIZE/2*CELL_SIZE;
		}
		if (bestPot < 127)
		{
			map[xSend][ySend].pot++;
		}
		Pathtracking_addWaypoint(&wp);
	} while(1);
}


