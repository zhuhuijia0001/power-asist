
#ifndef _METER_H_
#define _METER_H_

//R = 10mR, max current = 5A. LSB = 0.1mA
#define  CALIBRATION_VAL     5120

extern void InitMeter(uint16 cali);

extern bool GetBusVoltage(uint8 *VInt, uint16 *VFrac);

extern bool GetShuntVoltage(uint8 *mVInt, uint16 *mVFrac);

extern bool GetLoadCurrent(uint8 *AInt, uint16 *AFrac);

extern bool GetLoadPower(uint8 *WInt, uint16 *WFrac);

extern bool SetCalibrationVal(uint16 theoreticalCur, uint16 realCur);

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

