#include "hal_types.h"
#include "comdef.h"

#include "EEPROM.h"
#include "Parameter.h"

#include "Meter.h"
#include "LCD.h"

#include "npi.h"

#define  HEADER     0xA5

#define  PARAMETER_START_ADDR      0

StoreVector g_storeVector;


static const Parameter s_defaultParameter = 
{
	.currentCaliValue = CALIBRATION_VAL,

	.bleName = { 'B', 'L', 'E', ' ', 'P', 'o', 'w', 'e', 'r', ' ', 
 				 'A', 's', 'i', 's', 't', '\0', '\0', '\0', '\0', '\0', 
 				 '\0'},

 	.bleOn = BLE_ON,

	.mainMenu = 0,

	.autoDetect = 0,
	
 	.screenLockTime = LOCK_MAX,

 	.screenAngle = ORIENTATION_0,

 	.sampleRate = SAMPLE_MIN,

 	.peakValleySampleDuration = SAMPLE_DURATION_MAX,
};

bool LoadParameter()
{
	uint16 addr = PARAMETER_START_ADDR;

	uint8 dat;	
	
	//header
	if (!ReadEEPROM(addr, &dat))
	{		
		return false;
	}

	if (dat != HEADER)
	{	
		return false;
	}

	//size
	uint16 size;
	addr += sizeof(dat);
	if (!ReadEEPROMData(addr, (uint8 *)&size, sizeof(size), NULL))
	{		
		return false;
	}

	if (size != sizeof(Parameter))
	{		
		return false;
	}

	//parameter
	addr += sizeof(size);
	if (!ReadEEPROMData(addr, (uint8 *)&g_storeVector.parameter, size, NULL))
	{
		return false;
	}

	//sum
	uint8 sum = 0x00;
	uint8 *p = (uint8 *)&g_storeVector.parameter;
	int i;
	for (i = 0; i < size; i++)
	{
		sum ^= *p++;
	}
	
	addr += size;
	if (!ReadEEPROM(addr, &dat))
	{		
		return false;
	}
	
	if (sum != dat)
	{		
		return false;
	}

	TRACE("load parameter OK\r\n");
	
	return true;
}

void LoadDefaultParameter()
{
	g_storeVector.parameter = s_defaultParameter;
}

bool SaveParameter()
{
  	uint16 addr = PARAMETER_START_ADDR;

	g_storeVector.header = HEADER;
	g_storeVector.size = sizeof(g_storeVector.parameter);

	uint8 *p = (uint8 *)&g_storeVector.parameter;
	uint8 sum = 0x00;
	for (int i = 0; i < sizeof(g_storeVector.parameter); i++)
	{	
		sum ^= *p++;
	}

	g_storeVector.sum = sum;

	if (!WriteEEPROMData(addr, (uint8 *)&g_storeVector, sizeof(g_storeVector)))
	{
		return false;
	}
	
	TRACE("save parameter OK\r\n");
	
	return true;
}


