
#ifndef _FM24CXX_H_
#define _FM24CXX_H_

#define EEPROM_TYPE_FM24CL04  0

#define EEPROM_TYPE_FM24CL16  1
#define EEPROM_TYPE_FM24CL64  2

#define EEPROM_TYPE    EEPROM_TYPE_FM24CL16

#if EEPROM_TYPE == EEPROM_TYPE_FM24CL04
#define EEPROM_PIN_A2     0
#define EEPROM_PIN_A1     0
#define EEPROM_PIN_A0     0

#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL16
#define EEPROM_PIN_A2     0
#define EEPROM_PIN_A1     0
#define EEPROM_PIN_A0     0

#elif EEPROM_TYPE == EEPROM_TYPE_FM24CL64
#define EEPROM_PIN_A2     0
#define EEPROM_PIN_A1     0
#define EEPROM_PIN_A0     0

#endif 

extern bool DetectFM24Cxx();

extern bool WriteFM24CxxByte(uint16 address, uint8 dat);
extern bool ReadFM24CxxByte(uint16 address, uint8 *pData);

extern bool WriteFM24CxxData(uint16 address, const uint8 *pData, uint16 len);
extern bool ReadFM24CxxData(uint16 address, uint8 *pData, uint16 len, uint16 *pRetLen);


#endif

