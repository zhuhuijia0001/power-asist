
#include "comdef.h"
#include "hal_types.h"

#include "I2CDevice.h"
#include "FM24Cxx.h"

#define FM24CXX_ADDRESS      0xA0

bool DetectFM24Cxx()
{
#if EEPROM_TYPE == EEPROM_TYPE_FM24CL04
	uint8 dev = FM24CXX_ADDRESS
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2);
				
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL16
	uint8 dev = FM24CXX_ADDRESS;

#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL64
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (EEPROM_PIN_A0 << 1);
#else
	#error "invalid device"
#endif

	return DetectI2CDevice(dev);
}

bool WriteFM24CxxByte(uint16 address, uint8 dat)
{
	uint8 addr[2];
	uint8 addrLen;

#if EEPROM_TYPE == EEPROM_TYPE_FM24CL04	
	uint8 addrHigh = (address >> 8) & 0x01;
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;
	
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL16

	uint8 addrHigh = (address >> 8) & 0x07;
	uint8 dev = FM24CXX_ADDRESS | (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;

#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL64
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (EEPROM_PIN_A0 << 1);
				
	addr[0] = address >> 8;
	addr[1] = address;
	addrLen = 2;
#else
	#error "invalid device"
#endif

	return WriteI2CDeviceData(dev, addr, addrLen, &dat, sizeof(dat));
}

bool ReadFM24CxxByte(uint16 address, uint8 *pData)
{
	uint8 addr[2];
	uint8 addrLen;

#if EEPROM_TYPE == EEPROM_TYPE_FM24CL04	
	uint8 addrHigh = (address >> 8) & 0x01;
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;
	
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL16
	
	uint8 addrHigh = (address >> 8) & 0x07;
	uint8 dev = FM24CXX_ADDRESS | (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;
	
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL64
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (EEPROM_PIN_A0 << 1);
				
	addr[0] = address >> 8;
	addr[1] = address;
	addrLen = 2;

#else
	#error "invalid device"

#endif

	return ReadI2CDeviceData(dev, addr, addrLen, pData, sizeof(*pData), NULL);
}

bool WriteFM24CxxData(uint16 address, const uint8 *pData, uint16 len)
{
	uint8 addr[2];
	uint8 addrLen;

#if EEPROM_TYPE == EEPROM_TYPE_FM24CL04	
	uint8 addrHigh = (address >> 8) & 0x01;
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;
	
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL16
		
	uint8 addrHigh = (address >> 8) & 0x07;
	uint8 dev = FM24CXX_ADDRESS | (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;
		
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL64
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (EEPROM_PIN_A0 << 1);
				
	addr[0] = address >> 8;
	addr[1] = address;
	addrLen = 2;

#else
	#error "invalid device"

#endif
	
	return WriteI2CDeviceData(dev, addr, addrLen, pData, len);
}

bool ReadFM24CxxData(uint16 address, uint8 *pData, uint16 len, uint16 *pRetLen)
{
	uint8 addr[2];
	uint8 addrLen;

#if EEPROM_TYPE == EEPROM_TYPE_FM24CL04	
	uint8 addrHigh = (address >> 8) & 0x01;
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;
	
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL16
		
	uint8 addrHigh = (address >> 8) & 0x07;
	uint8 dev = FM24CXX_ADDRESS | (addrHigh << 1);

	addr[0] = address & 0xff;
	addrLen = 1;
		
#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL64
	uint8 dev = FM24CXX_ADDRESS 
				| (EEPROM_PIN_A2 << 3) 
				| (EEPROM_PIN_A1 << 2) 
				| (EEPROM_PIN_A0 << 1);
				
	addr[0] = address >> 8;
	addr[1] = address;
	addrLen = 2;

#else
	#error "invalid device"

#endif

	return ReadI2CDeviceData(dev, addr, addrLen, pData, len, pRetLen);
}

