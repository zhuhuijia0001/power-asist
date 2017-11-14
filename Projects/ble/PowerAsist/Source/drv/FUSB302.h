
#ifndef _FUSB302_H_
#define _FUSB302_H_

//registers
#define FUSB302_REG_DEVICEID            0x01

#define PD_HEADER_CNT(header)      (((header) >> 12) & 0x07)
#define PD_HEADER_TYPE(header)       ((header) & 0x1F)
#define PD_HEADER_ID(header)      (((header) >> 9) & 0x07)

// parity
typedef enum
{
	parity_none = 0,
	parity_cc1,
	parity_cc2
} CC_PARITY;

extern bool DetectFUSB302();

extern uint8 GetFUSB302DeviceID();

extern void InitFUSB302();

extern void ProcessFUSB302Interrupt(void);

extern void PDTransmit(uint16 header, const uint32 *pdo, uint8 cnt);
extern void PDRegisterRecvCallback(void (*callback)(uint16, const uint32 *, uint8));

extern CC_PARITY GetCCParity();

extern void ConfigFUSB302();

extern void ResetFUSB302();

#endif

