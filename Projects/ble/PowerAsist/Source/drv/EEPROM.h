
#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "FM24Cxx.h"

#define EEPROM_SIZE       (2 * 1024)

#define WriteEEPROM(addr, dat)     WriteFM24CxxByte(addr, dat)

#define ReadEEPROM(addr, pData)    ReadFM24CxxByte(addr, pData)

#define WriteEEPROMData(addr, pData, len)  WriteFM24CxxData(addr, pData, len)

#define ReadEEPROMData(addr, pData, len, pRetLen)  ReadFM24CxxData(addr, pData, len, pRetLen)

#endif

