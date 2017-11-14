
#include "hal_types.h"

#include "PinDefine.h"

#include "OSAL.h"

#include "hal_drivers.h"

#include "Meter.h"

#include "QuickCharge.h"

#include "npi.h"

#define QC_SUPPORT_CHECK_PERIOD   200
#define QC_SUPPORT_MAX_CHECK_COUNT 10

//zero threshould
#define QC_DM_ZERO_THREADSHOLD     10

static uint8 s_checkCount = 0;

static void (*s_supportCallback)(bool supported) = NULL;

static void Delay(void)
{
    volatile uint16 i = 120;
    do 
    {
    } while (--i);
}

void InitQC()
{
	//set tristate
	P1INP |= (1 << GET_GPIO_OFFSET(PIN_QC_DRIVE_A)) 
			| (1 << GET_GPIO_OFFSET(PIN_QC_DRIVE_B))
			| (1 << GET_GPIO_OFFSET(PIN_QC_DRIVE_C)) 
			| (1 << GET_GPIO_OFFSET(PIN_QC_DRIVE_D));
	
	SET_GPIO_INPUT(PIN_QC_DRIVE_A);
	SET_GPIO_INPUT(PIN_QC_DRIVE_B);
	SET_GPIO_INPUT(PIN_QC_DRIVE_C);
	SET_GPIO_INPUT(PIN_QC_DRIVE_D);
}

static void SetDP3_3V()
{
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_A);
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_B);
	
	SET_GPIO_BIT(PIN_QC_DRIVE_A);
	SET_GPIO_BIT(PIN_QC_DRIVE_B);
}

static void SetDP0_6V()
{
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_A);
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_B);
	
	SET_GPIO_BIT(PIN_QC_DRIVE_A);
	CLR_GPIO_BIT(PIN_QC_DRIVE_B);
}

static void SetDM3_3V()
{
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_C);
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_D);
	
	SET_GPIO_BIT(PIN_QC_DRIVE_C);
	SET_GPIO_BIT(PIN_QC_DRIVE_D);
}

static void SetDM0_6V()
{
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_C);
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_D);
	
	SET_GPIO_BIT(PIN_QC_DRIVE_C);
	CLR_GPIO_BIT(PIN_QC_DRIVE_D);
}

static void SetDM0V()
{
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_C);
	SET_GPIO_OUTPUT(PIN_QC_DRIVE_D);
	
	CLR_GPIO_BIT(PIN_QC_DRIVE_C);
	CLR_GPIO_BIT(PIN_QC_DRIVE_D);
}

void StartQC20Sniffer(void (*callback)(bool supported))
{
	SetDP0_6V();

	s_supportCallback = callback;

	s_checkCount = 0;
	osal_start_reload_timer(Hal_TaskID, QC_CHECK_SUPPORT_EVT, QC_SUPPORT_CHECK_PERIOD);
}

void CheckQC20Supported()
{
	s_checkCount++;
	if (s_checkCount > QC_SUPPORT_MAX_CHECK_COUNT)
	{
		osal_stop_timerEx(Hal_TaskID, QC_CHECK_SUPPORT_EVT);
		
		if (s_supportCallback != NULL)
		{
			s_supportCallback(false);
		}
	}
	else 
	{
		uint8 integer;
		uint16 frac;
		GetDMVoltage(&integer, &frac);
		//TRACE("integer:%d,frac:%02d\r\n", integer, frac);
		
		if (integer == 0 && frac < QC_DM_ZERO_THREADSHOLD)
		{
			osal_stop_timerEx(Hal_TaskID, QC_CHECK_SUPPORT_EVT);
			if (s_supportCallback != NULL)
			{
				s_supportCallback(true);
			}
		}
	}
}

void StopQC20Sniffer()
{
	SET_GPIO_INPUT(PIN_QC_DRIVE_A);
	SET_GPIO_INPUT(PIN_QC_DRIVE_B);

	SET_GPIO_INPUT(PIN_QC_DRIVE_C);
	SET_GPIO_INPUT(PIN_QC_DRIVE_D);
}

void SetQC20Sniffer5V()
{
	SetDP0_6V();
	SetDM0V();
}

void SetQC20Sniffer9V()
{
	SetDP3_3V();
	SetDM0_6V();
}

void SetQC20Sniffer12V()
{
	SetDP0_6V();
	SetDM0_6V();
}

void SetQC20Sniffer20V()
{
	SetDP3_3V();
	SetDM3_3V();
}

void SetQC30Mode()
{
	SetDP0_6V();
	SetDM3_3V();
}

void IncreaseQC30Voltage()
{
	SetDP3_3V();

	Delay();
	SetDP0_6V();
}

void DecreaseQC30Voltage()
{
	SetDM0_6V();

	Delay();
	SetDM3_3V();
}

