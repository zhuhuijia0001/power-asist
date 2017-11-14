
#include "comdef.h"
#include "hal_types.h"

#include "I2CDevice.h"
#include "INA226.h"

//address
#define INA226_ADDRESS   0x80

bool DetectINA226()
{
	return DetectI2CDevice(INA226_ADDRESS);
}

bool WriteINA226Data(uint8 addr, uint16 dat)
{
	uint8 buf[2];
	buf[0] = dat >> 8;
	buf[1] = dat;

	return WriteI2CDeviceData(INA226_ADDRESS, &addr, sizeof(addr), buf, sizeof(buf));
}

bool SetINA226Addr(uint8 addr)
{
	return WriteI2CDeviceData(INA226_ADDRESS, &addr, sizeof(addr), NULL, 0);
}

bool ReadINA226Data(uint16 *pData)
{
	uint8 buf[2];

	if (ReadI2CDeviceData(INA226_ADDRESS, NULL, 0, buf, sizeof(buf), NULL))
	{
		if (pData != NULL)
		{
			*pData = (buf[0] << 8) | buf[1];
		}

		return true;
	}

	return false;
}

