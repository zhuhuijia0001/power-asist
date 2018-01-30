#include "bcomdef.h"
#include "hal_assert.h"

#include "PowerAsist.h"

#include "Calibration.h"

#define CALI_TIMER_INTERVAL    200ul

static uint16 s_timerId = 0;

static CALI_STEP_CALLBACK  s_caliStepCallback = NULL;
static CALI_FINISH_CALLBACK s_caliFinishCallback = NULL;

static GET_ADC_FUN s_getAdcFun = NULL;

#define CALI_SAMPLE_COUNT    10

static uint8 s_caliSampleCount = 0;

static uint32 s_adcSum = 0;
static uint32 s_adcMin = 0;
static uint32 s_adcMax = 0;

void SetCalibrationCallback(CALI_STEP_CALLBACK stepCallback, CALI_FINISH_CALLBACK finishCallback)
{
	s_caliStepCallback = stepCallback;

	s_caliFinishCallback = finishCallback;
}

void StartCalibration(uint16 timerId, GET_ADC_FUN fun)
{
	HAL_ASSERT(fun != NULL);

	s_timerId = timerId;
	
	s_getAdcFun = fun;

	s_caliSampleCount = 0;
	s_adcSum = 0;
	s_adcMin = 0;
	s_adcMax = 0;

	StartPowerAsistTimer(timerId, CALI_TIMER_INTERVAL, true);
}

void CalibrationTimerProcess()
{
	uint16 adc;
	if (s_getAdcFun(&adc))
	{
		s_adcSum += adc;

		if (s_caliSampleCount == 0)
		{
			s_adcMin = s_adcMax = adc;
		}
		else
		{
			if (adc < s_adcMin)
			{
				s_adcMin = adc;
			}

			if (adc > s_adcMax)
			{
				s_adcMax = adc;
			}
		}
	}

	s_caliSampleCount++;
	
	if (s_caliStepCallback != NULL)
	{
		s_caliStepCallback(s_caliSampleCount * 100 / CALI_SAMPLE_COUNT);
	}
	
	if (s_caliSampleCount == CALI_SAMPLE_COUNT)
	{
		if (s_caliFinishCallback != NULL)
		{
			s_adcSum -= s_adcMin + s_adcMax;

			s_adcSum /= CALI_SAMPLE_COUNT - 2;
		
			s_caliFinishCallback(s_adcSum);
		}

		StopPowerAsistTimer(s_timerId);
	}
}

