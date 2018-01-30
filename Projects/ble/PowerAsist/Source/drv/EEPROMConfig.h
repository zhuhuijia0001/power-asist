
#ifndef _EEPROM_CONFIG_H_
#define _EEPROM_CONFIG_H_

//parameter start address
#define  PARAMETER_START_ADDR      0

//real time data address 
#define REAL_TIME_DATA_ADDR_A       1024
//real time data backup address
#define REAL_TIME_DATA_ADDR_B       (REAL_TIME_DATA_ADDR_A + 128)

//calibration data start address
#define CALIBRATION_START_ADDR     512

#endif
