
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

#define SAMPLE_PER_SECOND    10ul

#define SECONDS_PER_HOUR     3600ul

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

void InitMeter(uint16 cali)
{
	//Init ina226
	uint16 config = (0x04 << 12)
					| (0x02 << 9)   //number of average is 16
					| (0x04 << 6)   //vbus conversion time 1.1ms
					| (0x04 << 3)   //shunt conversion time is 1.1ms
					| (0x07 << 0)   //shunt and bus, continuous
					;
	
	WriteINA226Data(INA226_REG_CONFIG, config);
	
	WriteINA226Data(INA226_REG_CALIBRATION, cali);
	
	//init adc to measure d+ & d-
	HalAdcInit();
}

bool GetBusVoltage(uint8 *VInt, uint16 *VFrac)
{
	uint16 val;
	SetINA226Addr(INA226_REG_BUS_VOLTAGE);
			
	bool res = ReadINA226Data(&val);
	if (!res)
	{
		return false;
	}

	//TRACE("voltage adc:0x%04X\r\n", val);
	
	uint32 voltage = val ;
	//LSB = 1.25mV
	voltage *= 1250ul;

	if (VInt != NULL)
	{
		*VInt = voltage / 1000000;
	}

	if (VFrac != NULL)
	{
		*VFrac = voltage % 1000000 / 100;
	}

	return true;
}

bool GetShuntVoltage(uint8 *mVInt, uint16 *mVFrac)
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
	
	//LSB = 2.5uV
	uint32 voltage = val;
	voltage = (voltage * 250ul + 5) / 10;
	
	if (mVInt != NULL)
	{
		*mVInt = voltage / 10000;
	}

	if (mVFrac != NULL)
	{
		*mVFrac = voltage % 10000;
	}

	return true;
}

bool GetLoadCurrent(uint8 *AInt, uint16 *AFrac)
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
	
	//LSB = 0.1mA
	uint32 current = val;
	current *= 100ul;
	
	if (AInt != NULL)
	{
		*AInt = current / 1000000;
	}

	if (AFrac != NULL)
	{
		*AFrac = current % 1000000 / 100;
	}

	return true;
}

bool GetLoadPower(uint8 *WInt, uint16 *WFrac)
{
	uint16 val;
	
	SetINA226Addr(INA226_REG_POWER);
	bool res = ReadINA226Data(&val);
	if (!res)
	{
		return false;
	}
	
	//LSB = 2.5mW
	uint32 power = val;
	power *= 2500ul;
	
	if (WInt != NULL)
	{
		*WInt = power / 1000000;
	}

	if (WFrac != NULL)
	{
		*WFrac = power % 1000000 / 100;
	}

	return true;
}

bool SetCalibrationVal(uint16 theoreticalCur, uint16 realCur)
{
	uint32 cali = CALIBRATION_VAL * realCur / theoreticalCur;

	return WriteINA226Data(INA226_REG_CALIBRATION, cali);
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
		*VInt = val / 1000;
	}

	if (VFrac != NULL)
	{
		*VFrac = val / 10 % 100;
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

void StartAccumulateWhAndAh()
{
	s_prescaleCounter = SAVE_REAL_DATA_PRESCALE;
	
	osal_start_reload_timer(Hal_TaskID, SAMPLE_A_W_EVENT, 1000ul / SAMPLE_PER_SECOND);
}

void StopAccumulateWhAndAh()
{
	s_prescaleCounter = 0;
	
	osal_stop_timerEx(Hal_TaskID, SAMPLE_A_W_EVENT);
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
		s_WhAccumulate.sum -= SAMPLE_PER_SECOND * SECONDS_PER_HOUR;

		s_WhAccumulate.valh++;
	}
	
	uint32 A = AInt * 10000 + AFrac;
	s_AhAccumulate.sum += A;
	
	if (s_AhAccumulate.sum >= SAMPLE_PER_SECOND * SECONDS_PER_HOUR)
	{
		s_AhAccumulate.sum -= SAMPLE_PER_SECOND * SECONDS_PER_HOUR;

		s_AhAccumulate.valh++;
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
		*WhInt = s_WhAccumulate.valh / 10000;
	}

	if (WhFrac != NULL)
	{
		*WhFrac = s_WhAccumulate.valh % 10000;
	}
}

void GetLoadAh(uint16 *AhInt, uint16 *AhFrac)
{
	//TRACE("A sum:%lu\r\n", s_AhAccumulate.sum);
	
	if (AhInt != NULL)
	{
		*AhInt = s_AhAccumulate.valh / 10000;
	}

	if (AhFrac != NULL)
	{
		*AhFrac = s_AhAccumulate.valh % 10000;
	}
}

