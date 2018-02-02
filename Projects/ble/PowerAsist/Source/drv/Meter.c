
#include "comdef.h"
#include "hal_types.h"

#include "OSAL.h"

#include "hal_drivers.h"
#include "hal_adc.h"

#include "INA226.h"
#include "RealTimeData.h"
#include "Meter.h"

#include "npi.h"

//ref voltage 3300mV
#define REF_VOLTAGE      3300ul

#define SAMPLE_PER_SECOND    5ul

#define SECONDS_PER_HOUR     3600ul

//R = 10mR, max current = 5A. LSB = 0.1mA
#define CURRENT_CALIBRATION_VAL     5120ul

static AccumulateStruct s_WhAccumulate = 
{
	.valh = 0,
	.sum = 0,
};

static AccumulateStruct s_AhAccumulate = 
{
	.valh = 0,
	.sum = 0,
};

#define SAVE_REAL_DATA_PRESCALE           5
static uint8 s_prescaleCounter = 0;

//voltage method
typedef enum
{
	VOLTAGE_METHOD_NORMAL = 0,

	VOLTAGE_METHOD_CALIBRATED,
} voltage_method;

static voltage_method s_voltageMethod = VOLTAGE_METHOD_NORMAL;

//current method
typedef enum
{
	CURRENT_METHOD_NORMAL = 0,

	CURRENT_METHOD_CALIBRATED,
} current_method;

static current_method s_currentMethod = CURRENT_METHOD_NORMAL;

//power method
typedef enum
{
	POWER_METHOD_NORMAL = 0,
	
	POWER_METHOD_CALIBRATED,
} power_method;

static const CalibrationItem *s_voltageCaliItem = NULL;

static const CalibrationItem *s_currentCaliItem = NULL;

void InitMeter()
{
	//Init ina226
	uint16 config = (0x04 << 12)
					| (0x02 << 9)   //number of average is 16
					| (0x04 << 6)   //vbus conversion time 1.1ms
					| (0x04 << 3)   //shunt conversion time is 1.1ms
					| (0x07 << 0)   //shunt and bus, continuous
					;
	
	WriteINA226Data(INA226_REG_CONFIG, config);
	
	WriteINA226Data(INA226_REG_CALIBRATION, CURRENT_CALIBRATION_VAL);
	
	//init adc to measure d+ & d-
	HalAdcInit();
}

void SetBusVoltageCalibrationItem(const CalibrationItem *caliItem, uint8 count)
{
	if (count == VOLTAGE_CALI_ITEM_COUNT)
	{
		s_voltageCaliItem = caliItem;

		s_voltageMethod = VOLTAGE_METHOD_CALIBRATED;
	}
	else
	{
		s_voltageCaliItem = NULL;

		s_voltageMethod = VOLTAGE_METHOD_NORMAL;
	}
}


void SetLoadCurrentCalibrationItem(const CalibrationItem *caliItem, uint8 count)
{
	if (count == CURRENT_CALI_ITEM_COUNT)
	{
		s_currentCaliItem = caliItem;

		s_currentMethod = CURRENT_METHOD_CALIBRATED;
	}
	else
	{
		s_currentCaliItem = NULL;

		s_currentMethod = CURRENT_METHOD_NORMAL;
	}
}

bool GetBusVoltageAdcValue(uint16 *adc)
{
	SetINA226Addr(INA226_REG_BUS_VOLTAGE);

	bool res = ReadINA226Data(adc);
	if (!res)
	{
		return false;
	}
	
	return true;
}

static bool GetBusVoltageNormal(uint8 *VInt, uint16 *VFrac)
{
	uint16 adc;
	if (!GetBusVoltageAdcValue(&adc))
	{
		return false;
	}

	//TRACE("voltage adc:%d\r\n", adc);
		
	uint32 voltage = adc;
	//LSB = 1.25mV
	voltage *= 1250ul;
												 
	if (VInt != NULL)
	{
		*VInt = voltage / 1000000ul;
	}

	if (VFrac != NULL)
	{
		*VFrac = voltage % 1000000ul / 100ul;
	}
	
	return true;
}

static bool GetBusVoltageCalibrated(uint8 *VInt, uint16 *VFrac)
{
	uint16 adc;
	if (!GetBusVoltageAdcValue(&adc))
	{
		return false;
	}

	//TRACE("voltage adc:%d\r\n", adc);
	
	uint32 val;

	const CalibrationItem *caliItem = s_voltageCaliItem;

	if (adc >= caliItem[1].adc)
	{
		val = (caliItem[2].val.dec * 10000ul + caliItem[2].val.frac 
				- caliItem[1].val.dec * 10000ul - caliItem[1].val.frac);

		val *= (adc - caliItem[1].adc);
		val /= (caliItem[2].adc - caliItem[1].adc);

		val = (caliItem[1].val.dec * 10000ul + caliItem[1].val.frac + val);

		if (VInt != NULL)
		{
			*VInt = val / 10000ul;
		}

		if (VFrac != NULL)
		{
			*VFrac = val % 10000ul;
		}
	}
	else
	{
		val = (caliItem[1].val.dec * 10000ul + caliItem[1].val.frac
			- caliItem[0].val.dec * 10000ul - caliItem[0].val.frac);

		if (adc >= caliItem[0].adc)
		{
			val *= (adc - caliItem[0].adc);
			val /= (caliItem[1].adc - caliItem[0].adc);
			val = (caliItem[0].val.dec * 10000ul + caliItem[0].val.frac + val);

			if (VInt != NULL)
			{
				*VInt = val / 10000ul;
			}

			if (VFrac != NULL)
			{
				*VFrac = val % 10000ul;
			}
		}
		else
		{
			val *= (caliItem[0].adc - adc);
			val /= (caliItem[1].adc - caliItem[0].adc);
			val = (caliItem[0].val.dec * 10000ul + caliItem[0].val.frac - val);
			if (VInt != NULL)
			{
				*VInt = val / 10000ul;
			}

			if (VFrac != NULL)
			{
				*VFrac = val % 10000ul;
			}
		}
	}

	return true;
}

static bool (*const s_getBusVoltageFun[])(uint8 *VInt, uint16 *VFrac) = 
{
	[VOLTAGE_METHOD_NORMAL]     = GetBusVoltageNormal,
	[VOLTAGE_METHOD_CALIBRATED] = GetBusVoltageCalibrated,
};

bool GetBusVoltage(uint8 *VInt, uint16 *VFrac)
{
	return s_getBusVoltageFun[s_voltageMethod](VInt, VFrac);
}

bool GetShuntVoltageAdcValue(uint16 *adc)
{
	uint16 val;
	SetINA226Addr(INA226_REG_SHUNT_VOLTAGE);
			
	bool res = ReadINA226Data(&val);
	if (!res)
	{
		return false;
	}

	if (val & 0x8000)
	{
		val = ~val + 1;
	}
	
	if (adc != NULL)
	{
		*adc = val;
	}

	return true;
}

bool GetShuntVoltage(uint8 *mVInt, uint16 *mVFrac)
{
	uint16 adc;
	if (!GetShuntVoltageAdcValue(&adc))
	{
		return false;
	}
	
	//LSB = 2.5uV
	uint32 voltage = adc;
	voltage = (voltage * 250ul + 5) / 10;
	
	if (mVInt != NULL)
	{
		*mVInt = voltage / 10000ul;
	}

	if (mVFrac != NULL)
	{
		*mVFrac = voltage % 10000ul;
	}

	return true;
}

bool GetLoadCurrentAdcValue(uint16 *adc)
{
	uint16 val;

	SetINA226Addr(INA226_REG_CURRENT);
	bool res = ReadINA226Data(&val);
	if (!res)
	{
		return false;
	}
	
	if (val & 0x8000)
	{
		val = ~val + 1;
	}
	
	if (adc != NULL)
	{
		*adc = val;
	}

	return true;
}

static bool GetLoadCurrentNormal(uint8 *AInt, uint16 *AFrac)
{
	uint16 adc;

	if (!GetLoadCurrentAdcValue(&adc))
	{
		return false;
	}

	//TRACE("current adc:%d\r\n", adc);
		
	//LSB = 0.1mA
	uint32 current = adc;
	current *= 100ul;
	
	if (AInt != NULL)
	{
		*AInt = current / 1000000ul;
	}

	if (AFrac != NULL)
	{
		*AFrac = current % 1000000ul / 100ul;
	}

	return true;
}

static bool GetLoadCurrentCalibrated(uint8 *AInt, uint16 *AFrac)
{	
	uint16 adc;

	if (!GetLoadCurrentAdcValue(&adc))
	{
		return false;
	}

	//TRACE("current adc:%d\r\n", adc);

	uint32 val;

	const CalibrationItem *caliItem = s_currentCaliItem;

	if (adc >= caliItem[2].adc)
	{
		val = (caliItem[3].val.dec * 10000ul + caliItem[3].val.frac 
				- caliItem[2].val.dec * 10000ul - caliItem[2].val.frac);
	
		val *= (adc - caliItem[2].adc);
		val /= (caliItem[3].adc - caliItem[2].adc);

		val = (caliItem[2].val.dec * 10000ul + caliItem[2].val.frac + val);

		if (AInt != NULL)
		{
			*AInt = val / 10000ul;
		}

		if (AFrac != NULL)
		{
			*AFrac = val % 10000ul;
		}
	}
	else if (adc >= caliItem[1].adc)
	{
		val = (caliItem[2].val.dec * 10000ul + caliItem[2].val.frac 
							- caliItem[1].val.dec * 10000ul - caliItem[1].val.frac);
				
		val *= (adc - caliItem[1].adc);
		val /= (caliItem[2].adc - caliItem[1].adc);

		val = (caliItem[1].val.dec * 10000ul + caliItem[1].val.frac + val);

		if (AInt != NULL)
		{
			*AInt = val / 10000ul;
		}

		if (AFrac != NULL)
		{
			*AFrac = val % 10000ul;
		}
	}
	else
	{
		val = (caliItem[1].val.dec * 10000ul + caliItem[1].val.frac
			- caliItem[0].val.dec * 10000ul - caliItem[0].val.frac);
	
		val *= (adc - caliItem[0].adc);
		val /= (caliItem[1].adc - caliItem[0].adc);
		val = (caliItem[0].val.dec * 10000ul + caliItem[0].val.frac + val);

		if (AInt != NULL)
		{
			*AInt = val / 10000ul;
		}

		if (AFrac != NULL)
		{
			*AFrac = val % 10000ul;
		}
	}

	return true;
}

static bool (*const s_getLoadCurrentFun[])(uint8 *AInt, uint16 *AFrac) = 
{
	[CURRENT_METHOD_NORMAL]     = GetLoadCurrentNormal,
	[CURRENT_METHOD_CALIBRATED] = GetLoadCurrentCalibrated,
};

bool GetLoadCurrent(uint8 *AInt, uint16 *AFrac)
{
	return s_getLoadCurrentFun[s_currentMethod](AInt, AFrac);
}

static bool GetLoadPowerNormal(uint8 *WInt, uint16 *WFrac)
{
	uint16 adc;
	
	SetINA226Addr(INA226_REG_POWER);
	bool res = ReadINA226Data(&adc);
	if (!res)
	{
		return false;
	}
	
	//LSB = 2.5mW
	uint32 power = adc;
	power *= 2500ul;
	
	if (WInt != NULL)
	{
		*WInt = power / 1000000ul;
	}

	if (WFrac != NULL)
	{
		*WFrac = power % 1000000ul / 100ul;
	}

	return true;
}

static bool GetLoadPowerCalibrated(uint8 *WInt, uint16 *WFrac)
{
	uint8 VDec;
	uint16 VFrac;
	
	if (!s_getBusVoltageFun[s_voltageMethod](&VDec, &VFrac))
	{
		return false;
	}

	uint8 ADec;
	uint16 AFrac;
	if (!s_getLoadCurrentFun[s_currentMethod](&ADec, &AFrac))
	{
		return false;
	}
	
	//calculate
	uint8 dec;
	uint16 frac;

	uint32 val = VFrac;
	val *= AFrac;

	val /= 10000;
	frac = val;

	val = VDec;
	val *= AFrac;
	frac += val;

	val = ADec;
	val *= VFrac;
	frac += val;

	dec = frac / 10000ul;
	frac %= 10000ul;

	val = VDec;
	val *= ADec;
	dec += val;
	
	if (WInt != NULL)
	{
		*WInt = dec;
	}

	if (WFrac != NULL)
	{
		*WFrac = frac;
	}
	
	return true;
}

static bool (*const s_getLoadPowerFun[])(uint8 *WInt, uint16 *WFrac) = 
{
	[POWER_METHOD_NORMAL] = GetLoadPowerNormal,

	[POWER_METHOD_CALIBRATED] = GetLoadPowerCalibrated,
};

bool GetLoadPower(uint8 *WInt, uint16 *WFrac)
{
	power_method method = POWER_METHOD_NORMAL;
	if (s_voltageMethod == VOLTAGE_METHOD_CALIBRATED
		|| s_currentMethod == CURRENT_METHOD_CALIBRATED)
	{
		method = POWER_METHOD_CALIBRATED;
	}

	return s_getLoadPowerFun[method](WInt, WFrac);
}

static void GetADCVoltage(uint8 channel, uint8 *VInt, uint16 *VFrac)
{	
#if MONITOR_TEMPERATURE
	HalAdcSetReference(HAL_ADC_REF_AVDD);
#endif
	
	uint32 res = HalAdcRead(channel, HAL_ADC_RESOLUTION_12);

	uint32 val = res * REF_VOLTAGE / 2048;
	
	if (VInt != NULL)
	{
		*VInt = val / 1000ul;
	}

	if (VFrac != NULL)
	{
		*VFrac = val / 10 % 100ul;
	}
}

void GetDPVoltage(uint8 *VInt, uint16 *VFrac)
{
	GetADCVoltage(HAL_ADC_CHN_AIN1, VInt, VFrac);
}

void GetDMVoltage(uint8 *VInt, uint16 *VFrac)
{
	GetADCVoltage(HAL_ADC_CHN_AIN2, VInt, VFrac);
}

bool GetADCTemperature(uint8 *TInt, uint8 *TFrac)
{
#if MONITOR_TEMPERATURE
	HalAdcSetReference(HAL_ADC_REF_125V);
	
	uint32 res = HalAdcRead(HAL_ADC_CHN_TEMP, HAL_ADC_RESOLUTION_12);

	TRACE("temperature val:%d\r\n", res);

	res = (res * 100 - 136750) / 45;
	
	if (TInt != NULL)
	{
		*TInt = res / 10;
	}

	if (TFrac != NULL)
	{
		*TFrac = res % 10;
	}

	return true;

#else
	return false;
#endif
}

static wh_ah_status s_wh_ah_status = WH_AH_STATUS_STOPPED;

void StartAccumulateWhAndAh()
{
	s_prescaleCounter = SAVE_REAL_DATA_PRESCALE;
	
	osal_start_reload_timer(Hal_TaskID, SAMPLE_A_W_EVENT, 1000ul / SAMPLE_PER_SECOND);

	s_wh_ah_status = WH_AH_STATUS_STARTED; 
}

void StopAccumulateWhAndAh()
{
	s_prescaleCounter = 0;
	
	osal_stop_timerEx(Hal_TaskID, SAMPLE_A_W_EVENT);

	s_wh_ah_status = WH_AH_STATUS_STOPPED;
}

wh_ah_status GetWhAndAhStatus()
{
	return s_wh_ah_status;
}

void GetSavedLoadWhAndAh()
{
	ReadRealTimeData(&s_WhAccumulate, &s_AhAccumulate);
}

void AccumulateLoadWhAndAh()
{
	uint8 WInt, AInt;
	uint16 WFrac, AFrac;

	GetLoadPower(&WInt, &WFrac);
	GetLoadCurrent(&AInt, &AFrac);

	uint32 W = WInt * 10000 + WFrac;
	s_WhAccumulate.sum += W;
	if (s_WhAccumulate.sum >= SAMPLE_PER_SECOND * SECONDS_PER_HOUR)
	{
		s_WhAccumulate.valh += s_WhAccumulate.sum / (SAMPLE_PER_SECOND * SECONDS_PER_HOUR);
		s_WhAccumulate.sum %= (SAMPLE_PER_SECOND * SECONDS_PER_HOUR);
	}
	
	uint32 A = AInt * 10000 + AFrac;
	s_AhAccumulate.sum += A;
	
	if (s_AhAccumulate.sum >= SAMPLE_PER_SECOND * SECONDS_PER_HOUR)
	{
		s_AhAccumulate.valh += s_AhAccumulate.sum / (SAMPLE_PER_SECOND * SECONDS_PER_HOUR);
		s_AhAccumulate.sum %= (SAMPLE_PER_SECOND * SECONDS_PER_HOUR);
	}
	
	//save
	s_prescaleCounter--;
	if (s_prescaleCounter == 0)
	{
		s_prescaleCounter = SAVE_REAL_DATA_PRESCALE;
		
		SaveRealTimeData(&s_WhAccumulate, &s_AhAccumulate);
	}
}

void FlushLoadWhAndAh()
{
	s_WhAccumulate.valh = 0;
	s_WhAccumulate.sum = 0;

	s_AhAccumulate.valh = 0;
	s_AhAccumulate.sum = 0;

	//save 
	SaveInitRealTimeData(&s_WhAccumulate, &s_AhAccumulate);
}

void GetLoadWh(uint16 *WhInt, uint16 *WhFrac)
{
	//TRACE("W sum:%lu\r\n", s_WhAccumulate.sum);
	
	if (WhInt != NULL)
	{
		*WhInt = s_WhAccumulate.valh / 10000ul;
	}

	if (WhFrac != NULL)
	{
		*WhFrac = s_WhAccumulate.valh % 10000ul;
	}
}

void GetLoadAh(uint16 *AhInt, uint16 *AhFrac)
{
	//TRACE("A sum:%lu\r\n", s_AhAccumulate.sum);
	
	if (AhInt != NULL)
	{
		*AhInt = s_AhAccumulate.valh / 10000ul;
	}

	if (AhFrac != NULL)
	{
		*AhFrac = s_AhAccumulate.valh % 10000ul;
	}
}
