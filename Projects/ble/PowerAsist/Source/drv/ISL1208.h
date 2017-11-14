
#ifndef _ISL1208_H_
#define _ISL1208_H_

//registers
#define ISL1208_REG_SC      0x00
#define ISL1208_REG_MN      0x01
#define ISL1208_REG_HR      0x02
#define ISL1208_REG_DT      0x03
#define ISL1208_REG_MO      0x04
#define ISL1208_REG_YR      0x05
#define ISL1208_REG_DW      0x06

#define ISL1208_REG_SR      0x07
#define ISL1208_REG_INT     0x08
#define ISL1208_REG_ATR     0x0A
#define ISL1208_REG_DTR     0x0B

#define ISL1208_REG_SCA     0x0C
#define ISL1208_REG_MNA     0x0D
#define ISL1208_REG_HRA     0x0E
#define ISL1208_REG_DTA     0x0F
#define ISL1208_REG_MOA     0x10
#define ISL1208_REG_DWA     0x11

#define ISL1208_REG_USER1   0x12
#define ISL1208_REG_USER2   0x13

extern bool DetectISL1208();

extern bool WriteISL1208Byte(uint8 addr, uint8 dat);

extern bool ReadISL1208Byte(uint8 addr, uint8 *pData);

extern bool WriteISL1208Data(uint8 addr, const uint8 *pData, uint16 len);

extern bool ReadISL1208Data(uint8 addr, uint8 *pData, uint16 len, uint16 *pRetLen);

#endif

