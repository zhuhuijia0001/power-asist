
#include "hal_types.h"

#include "Delay.h"

void DelayMs(uint16 ms)
{
	volatile uint16 i,j;
		
	for (i=0; i<ms; i++)
		for (j=0; j<500; j++);
}

