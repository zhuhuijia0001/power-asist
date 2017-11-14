
#ifndef _INA226_H_
#define _INA226_H_

/* address of registers */
#define INA226_REG_CONFIG               0x00
#define INA226_REG_SHUNT_VOLTAGE        0x01
#define INA226_REG_BUS_VOLTAGE          0x02
#define INA226_REG_POWER                0x03
#define INA226_REG_CURRENT              0x04
#define INA226_REG_CALIBRATION          0x05
#define INA226_REG_MASK_ENABLE          0x06
#define INA226_REG_ALERT_LIMIT          0x07

#define INA226_REG_MANUFACTURE_ID       0xFE
#define INA226_REG_DIE_ID               0xFF

extern bool DetectINA226();

extern bool WriteINA226Data(uint8 addr, uint16 dat);

extern bool SetINA226Addr(uint8 addr);

extern bool ReadINA226Data(uint16 *pData);

#endif

