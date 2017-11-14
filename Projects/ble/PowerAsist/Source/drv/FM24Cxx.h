
#ifndef _FM24CXX_H_
#define _FM24CXX_H_

extern bool DetectFM24Cxx();

extern bool WriteFM24CxxByte(uint16 addr, uint8 dat);
extern bool ReadFM24CxxByte(uint16 addr, uint8 *pData);

extern bool WriteFM24CxxData(uint16 addr, const uint8 *pData, uint16 len);
extern bool ReadFM24CxxData(uint16 addr, uint8 *pData, uint16 len, uint16 *pRetLen);


#endif

