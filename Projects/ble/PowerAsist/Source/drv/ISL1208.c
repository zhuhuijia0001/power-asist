

#include "comdef.h"
#include "hal_types.h"

#include "I2CDevice.h"

#define ISL1208_ADDRESS   0xDE

bool DetectISL1208()
{
	return DetectI2CDevice(ISL1208_ADDRESS);
}

bool WriteISL1208Byte(uint8 addr, uint8 dat)
{
	return WriteI2CDeviceData(ISL1208_ADDRESS, &addr, sizeof(addr), &dat, sizeof(dat));
}

bool ReadISL1208Byte(uint8 addr, uint8 *pData)
{
	return ReadI2CDeviceData(ISL1208_ADDRESS, &addr, sizeof(addr), pData, sizeof(*pData), NULL);
}

bool WriteISL1208Data(uint8 addr, const uint8 *pData, uint16 len)
{
	return WriteI2CDeviceData(ISL1208_ADDRESS, &addr, sizeof(addr), pData, len);
}

bool ReadISL1208Data(uint8 addr, uint8 *pData, uint16 len, uint16 *pRetLen)
{
	return ReadI2CDeviceData(ISL1208_ADDRESS, &addr, sizeof(addr), pData, len, pRetLen);
}
