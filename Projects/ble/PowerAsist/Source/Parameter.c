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

 	.bleOn = BLE_OFF,

	.mainMenu = 0,

	.autoDetect = AUTO_DETECT_OFF,

 	.screenLockTime = LOCK_MAX,

 	.screenAngle = ORIENTATION_0,

 	.sampleRate = DEFAULT_SAMPLE_RATE,
};

bool LoadParameter()
{
	uint16 addr = PARAMETER_START_ADDR;

	//read
	if (!ReadEEPROMData(addr, (uint8 *)&g_storeVector, sizeof(g_storeVector), NULL))
	{
		return false;
	}

	//header
	if (g_storeVector.header != HEADER)
	{
		return false;
	}

	//size
	if (g_storeVector.size != sizeof(Parameter))
	{
		return false;
	}

	//sum
	uint8 sum = 0x00;
	uint8 *p = (uint8 *)&g_storeVector.parameter;
	for (int i = 0; i < g_storeVector.size; i++)
	{
		sum ^= *p++;
	}
	
	if (sum != g_storeVector.sum)
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


