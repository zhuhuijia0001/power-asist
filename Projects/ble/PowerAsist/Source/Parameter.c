#include "hal_types.h"
#include "comdef.h"

#include "EEPROM.h"
#include "EEPROMConfig.h"

#include "Parameter.h"

#include "Meter.h"
#include "LCD.h"

#include "npi.h"

#define PARAMETER_HEADER     0xA5

#define CALIBRATION_HEADER   0xA7

ParameterVector g_parameterVector;

CalibrationVector g_calibrationVector;

static const Parameter s_defaultParameter = 
{
	.bleName = { 'B', 'L', 'E', ' ', 'P', 'o', 'w', 'e', 'r', ' ', 
 				 'A', 's', 'i', 's', 't', '\0', '\0', '\0', '\0', '\0', 
 				 '\0'},

 	.bleOn = BLE_OFF,

	.mainMenu = 0,

	.autoDetect = AUTO_DETECT_OFF,

 	.screenLockTime = DEFAULT_LOCK_MIN,

 	.screenAngle = ORIENTATION_0,

 	.sampleRate = DEFAULT_SAMPLE_RATE,
};

static const CalibrationData s_defaultCalibration = 
{
	.voltageCaliItem = 
	{
		[0] = 
		{
			//5.0000V
			.adc = 4000ul,
			.val = 
			{
				.dec = 5,
				.frac = 0,
			},
		},

		[1] = 
		{
			//12.0000V
			.adc = 9600ul,
			.val = 
			{
				.dec = 12,
				.frac = 0,
			},
		},

		[2] = 
		{
			//24.0000V
			.adc = 19200ul,
			.val = 
			{
				.dec = 24,
				.frac = 0,
			},
		},
	},
	
	.voltageCaliItemCount = 3,

	.currentCaliItem = 
	{
		[0] = 
		{
			//0.0000A
			.adc = 0,
			.val = 
			{
				.dec = 0,
				.frac = 0,
			},
		},

		[1] = 
		{
			//0.1000A
			.adc = 1000ul,
			.val = 
			{
				.dec = 0,
				.frac = 1000ul,
			},
		},

		[2] = 
		{
			//1.0000A
			.adc = 10000ul,
			.val = 
			{
				.dec = 1,
				.frac = 0,
			},
		},

		[3] = 
		{
			//3.0000A
			.adc = 30000ul,
			.val = 
			{
				.dec = 3,
				.frac = 0,
			},
		},
	},
	
	.currentCaliItemCount = 4,
};

bool LoadParameter()
{
	uint16 addr = PARAMETER_START_ADDR;

	//read
	if (!ReadEEPROMData(addr, (uint8 *)&g_parameterVector, sizeof(g_parameterVector), NULL))
	{
		return false;
	}

	//header
	if (g_parameterVector.header != PARAMETER_HEADER)
	{
		return false;
	}

	//size
	if (g_parameterVector.size != sizeof(Parameter))
	{
		return false;
	}

	//sum
	uint8 sum = 0x00;
	uint8 *p = (uint8 *)&g_parameterVector.parameter;
	for (int i = 0; i < g_parameterVector.size; i++)
	{
		sum ^= *p++;
	}
	
	if (sum != g_parameterVector.sum)
	{
		return false;
	}

	TRACE("load parameter OK\r\n");
	
	return true;
}

void LoadDefaultParameter()
{
	g_parameterVector.parameter = s_defaultParameter;
}

bool SaveParameter()
{
  	uint16 addr = PARAMETER_START_ADDR;

	g_parameterVector.header = PARAMETER_HEADER;
	g_parameterVector.size = sizeof(g_parameterVector.parameter);

	uint8 *p = (uint8 *)&g_parameterVector.parameter;
	uint8 sum = 0x00;
	for (int i = 0; i < sizeof(g_parameterVector.parameter); i++)
	{	
		sum ^= *p++;
	}

	g_parameterVector.sum = sum;

	if (!WriteEEPROMData(addr, (uint8 *)&g_parameterVector, sizeof(g_parameterVector)))
	{
		return false;
	}
	
	TRACE("save parameter OK\r\n");
	
	return true;
}

bool LoadCalibration()
{
	uint16 addr = CALIBRATION_START_ADDR;

	//read
	if (!ReadEEPROMData(addr, (uint8 *)&g_calibrationVector, sizeof(g_calibrationVector), NULL))
	{
		return false;
	}

	//header
	if (g_calibrationVector.header != CALIBRATION_HEADER)
	{
		return false;
	}

	//size
	if (g_calibrationVector.size != sizeof(g_calibrationVector.calibrationData))
	{
		return false;
	}

	//sum
	uint8 sum = 0x00;
	uint8 *p = (uint8 *)&g_calibrationVector.calibrationData;
	for (int i = 0; i < g_calibrationVector.size; i++)
	{
		sum ^= *p++;
	}
	
	if (sum != g_calibrationVector.sum)
	{
		return false;
	}

	TRACE("load calibration OK\r\n");

	return true;
}

void LoadDefaultCalibration()
{
	g_calibrationVector.calibrationData= s_defaultCalibration;
}

bool SaveCalibration()
{
	uint16 addr = CALIBRATION_START_ADDR;

	g_calibrationVector.header = CALIBRATION_HEADER;
	g_calibrationVector.size = sizeof(g_calibrationVector.calibrationData);

	uint8 *p = (uint8 *)&g_calibrationVector.calibrationData;
	uint8 sum = 0x00;
	for (int i = 0; i < sizeof(g_calibrationVector.calibrationData); i++)
	{	
		sum ^= *p++;
	}

	g_calibrationVector.sum = sum;

	if (!WriteEEPROMData(addr, (uint8 *)&g_calibrationVector, sizeof(g_calibrationVector)))
	{
		return false;
	}
	
	TRACE("save calibration OK\r\n");
	
	return true;
}

