
#include "comdef.h"
#include "hal_types.h"

#include "I2CDevice.h"
#include "FM24Cxx.h"

#define FM24CXX_ADDRESS      0xA0

bool DetectFM24Cxx()
{
	return DetectI2CDevice(FM24CXX_ADDRESS);
}

bool WriteFM24CxxByte(uint16 addr, uint8 dat)
{
	uint8 dev = FM24CXX_ADDRESS | ((addr >> 7) & 0x0E);
	uint8 ad = addr & 0xff;

	return WriteI2CDeviceData(dev, &ad, sizeof(ad), &dat, sizeof(dat));
}

bool ReadFM24CxxByte(uint16 addr, uint8 *pData)
{
	uint8 dev = FM24CXX_ADDRESS | ((addr >> 7) & 0x0E);
	uint8 ad = addr & 0xff;
	
	return ReadI2CDeviceData(dev, &ad, sizeof(ad), pData, sizeof(*pData), NULL);
}

bool WriteFM24CxxData(uint16 addr, const uint8 *pData, uint16 len)
{
	uint8 dev = FM24CXX_ADDRESS | ((addr >> 7) & 0x0E);
	uint8 ad = addr & 0xff;
	
	return WriteI2CDeviceData(dev, &ad, sizeof(ad), pData, len);
}

bool ReadFM24CxxData(uint16 addr, uint8 *pData, uint16 len, uint16 *pRetLen)
{
	uint8 dev = FM24CXX_ADDRESS | ((addr >> 7) & 0x0E);
	uint8 ad = addr & 0xff;
	
	return ReadI2CDeviceData(dev, &ad, sizeof(ad), pData, len, pRetLen);
}

