#include "hal_types.h"

#include "OSAL.h"

#include "hal_drivers.h"

#include "Meter.h"
#include "Detect.h"

#include "npi.h"

#define D_CHECK_PERIOD         100
#define D_MAX_CHECK_COUNT       10

//zero threshould
#define DCP_ZERO_THREADSHOLD     5

#define APPLE_21A_DP_VOL       270
#define APPLE_21A_DM_VOL       200
#define APPLE_21A_THREADSHOULD  20

static uint8 s_checkCount = 0;

static void (*s_callback)(DType) = NULL;

void StartDetectDType(void (*callback)(DType type))
{
	s_callback = callback;
	s_checkCount = 0;
	
	osal_start_reload_timer(Hal_TaskID, DETECT_CHECK_EVT, D_CHECK_PERIOD);
}

void CheckDType()
{
	s_checkCount++;
	if (s_checkCount > D_MAX_CHECK_COUNT)
	{
		osal_stop_timerEx(Hal_TaskID, DETECT_CHECK_EVT);
		
		if (s_callback != NULL)
		{
			s_callback(d_type_other);
		}
	}
	else 
	{
		uint8 integer;
		uint16 frac;

		GetDPVoltage(&integer, &frac);
		//TRACE("D+:integer:%d,frac:%02d\r\n", integer, frac);
		uint16 voltageDp = integer * 100 + frac;
		
		GetDMVoltage(&integer, &frac);
		//TRACE("D-:integer:%d,frac:%02d\r\n", integer, frac);
		uint16 voltageDm = integer * 100 + frac;

		if (voltageDp <= voltageDm + DCP_ZERO_THREADSHOLD
			&& voltageDp + DCP_ZERO_THREADSHOLD >= voltageDm)
		{
			//dcp
			osal_stop_timerEx(Hal_TaskID, DETECT_CHECK_EVT);
			if (s_callback != NULL)
			{
				s_callback(d_type_dcp);
			}
		}
		else if (voltageDp <= APPLE_21A_DP_VOL + APPLE_21A_THREADSHOULD
				&& voltageDp + APPLE_21A_THREADSHOULD >= APPLE_21A_DP_VOL
				&& voltageDm <= APPLE_21A_DM_VOL + APPLE_21A_THREADSHOULD
				&& voltageDm + APPLE_21A_THREADSHOULD >= APPLE_21A_DM_VOL )
		{
			//apple 2.1A
			osal_stop_timerEx(Hal_TaskID, DETECT_CHECK_EVT);
			if (s_callback != NULL)
			{
				s_callback(d_type_apple_21A);
			}
		}
	}
}

