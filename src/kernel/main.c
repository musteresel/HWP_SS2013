#include "kernel/task.h"
#include "util/attribute.h"




ATTRIBUTE( naked ) void main(void)
{
	Multitasking_init();
}

