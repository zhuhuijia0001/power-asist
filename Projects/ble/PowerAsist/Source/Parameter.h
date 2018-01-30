
#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#define MAX_BLE_NAME_LEN     20

#define BLE_ON     1
#define BLE_OFF    0

//minute
#define LOCK_1_MIN   1
#define LOCK_2_MIN   2
#define LOCK_3_MIN   3
#define LOCK_4_MIN   4
#define LOCK_5_MIN   5
#define LOCK_NEVER   0xff

#define DEFAULT_LOCK_MIN     LOCK_5_MIN

#define AUTO_DETECT_ON       1
#define AUTO_DETECT_OFF      0

//sample rate
#define SAMPLE_RATE_100             100
#define SAMPLE_RATE_50              50
#define SAMPLE_RATE_20              20
#define SAMPLE_RATE_10              10
#define SAMPLE_RATE_5               5
#define SAMPLE_RATE_2               2
#define SAMPLE_RATE_1               1

#define DEFAULT_SAMPLE_RATE         SAMPLE_RATE_10

#define SAMPLE_DURATION_MAX        10

#define FIRMWARE_VER_LEN  8
#define FIRMWARE_VER      "0.1"

#define WEB_SITE          "https://pcb-layout.taobao.com"

typedef struct 
{
	uint8   bleName[MAX_BLE_NAME_LEN + 1];  //name

	uint8   bleOn;   //turn on/off ble

	uint8   mainMenu; //main menu index

	uint8   autoDetect; //first auto detect
	
	uint8   screenLockTime; //screen lock time

	uint8   screenAngle;  //screen angle

	uint8   sampleRate;  //sample rate, counts per second
} Parameter;

typedef struct
{
	uint8  header;

	uint16 size;

	Parameter parameter;

	uint8  sum;
} ParameterVector;

extern ParameterVector g_parameterVector;

#define g_bleName            g_parameterVector.parameter.bleName
#define g_bleOn              g_parameterVector.parameter.bleOn
#define g_mainMenu           g_parameterVector.parameter.mainMenu
#define g_autoDetect         g_parameterVector.parameter.autoDetect
#define g_screenLockTime     g_parameterVector.parameter.screenLockTime
#define g_screenAngle        g_parameterVector.parameter.screenAngle
#define g_sampleRate         g_parameterVector.parameter.sampleRate

typedef struct
{
	uint8 dec;
	uint16 frac;
} CalibrationValue;

typedef struct
{
	CalibrationValue val;

	uint16 adc;
} CalibrationItem;

#define VOLTAGE_CALI_ITEM_COUNT        3
#define CURRENT_CALI_ITEM_COUNT        4

typedef struct 
{
	CalibrationItem  voltageCaliItem[VOLTAGE_CALI_ITEM_COUNT];
	uint8            voltageCaliItemCount;

	CalibrationItem  currentCaliItem[CURRENT_CALI_ITEM_COUNT];
	uint8            currentCaliItemCount;
} CalibrationData;

typedef struct
{
	uint8  header;

	uint16 size;

	CalibrationData calibrationData;

	uint8  sum;
} CalibrationVector;

extern CalibrationVector g_calibrationVector;

#define g_voltageCaliItem      g_calibrationVector.calibrationData.voltageCaliItem
#define g_voltageCaliItemCount g_calibrationVector.calibrationData.voltageCaliItemCount
#define g_currentCaliItem      g_calibrationVector.calibrationData.currentCaliItem
#define g_currentCaliItemCount g_calibrationVector.calibrationData.currentCaliItemCount

extern bool LoadParameter();
extern void LoadDefaultParameter();

extern bool SaveParameter();

extern bool LoadCalibration();
extern void LoadDefaultCalibration();

extern bool SaveCalibration();


#endif

