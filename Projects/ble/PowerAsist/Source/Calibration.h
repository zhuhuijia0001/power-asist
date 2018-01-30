
#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_


typedef bool (*GET_ADC_FUN)(uint16 *adc);

typedef void (*CALI_STEP_CALLBACK)(uint8 percent);
typedef void (*CALI_FINISH_CALLBACK)(uint16 caliAdc);

extern void SetCalibrationCallback(CALI_STEP_CALLBACK stepCallback, CALI_FINISH_CALLBACK finishCallback);

extern void StartCalibration(uint16 timerId, GET_ADC_FUN fun);

extern void CalibrationTimerProcess();

#endif

