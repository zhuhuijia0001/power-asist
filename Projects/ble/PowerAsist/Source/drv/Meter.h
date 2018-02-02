
#ifndef _METER_H_
#define _METER_H_

#include "Parameter.h"

extern void InitMeter();

extern void SetBusVoltageCalibrationItem(const CalibrationItem *caliItem, uint8 count);
extern void SetLoadCurrentCalibrationItem(const CalibrationItem *caliItem, uint8 count);

extern bool GetBusVoltageAdcValue(uint16 *adc);
extern bool GetBusVoltage(uint8 *VInt, uint16 *VFrac);

extern bool GetShuntVoltageAdcValue(uint16 *adc);
extern bool GetShuntVoltage(uint8 *mVInt, uint16 *mVFrac);

extern bool GetLoadCurrentAdcValue(uint16 *adc);
extern bool GetLoadCurrent(uint8 *AInt, uint16 *AFrac);

extern bool GetLoadPower(uint8 *WInt, uint16 *WFrac);

extern void GetDPVoltage(uint8 *VInt, uint16 *VFrac);

extern void GetDMVoltage(uint8 *VInt, uint16 *VFrac);

extern bool GetADCTemperature(uint8 *TInt, uint8 *TFrac);

//ah wh status
typedef enum
{
	WH_AH_STATUS_STARTED = 0,
	WH_AH_STATUS_STOPPED,
} wh_ah_status;

extern void StartAccumulateWhAndAh();
extern void StopAccumulateWhAndAh();

extern wh_ah_status GetWhAndAhStatus();

extern void GetSavedLoadWhAndAh();
extern void AccumulateLoadWhAndAh();

extern void FlushLoadWhAndAh();

extern void GetLoadWh(uint16 *WhInt, uint16 *WhFrac);
extern void GetLoadAh(uint16 *AhInt, uint16 *AhFrac);

#endif

