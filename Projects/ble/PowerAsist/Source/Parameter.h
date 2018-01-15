
#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#define MAX_BLE_NAME_LEN     20

#define BLE_ON     1
#define BLE_OFF    0

#define LOCK_MIN  10
#define LOCK_MAX  60

//sample rate
#define SAMPLE_RATE_20              20
#define SAMPLE_RATE_10              10
#define SAMPLE_RATE_5               5
#define SAMPLE_RATE_2               2
#define SAMPLE_RATE_1               1

#define DEFAULT_SAMPLE_RATE         SAMPLE_RATE_10

#define SAMPLE_DURATION_MIN   10
#define SAMPLE_DURATION_MAX   60

#define FIRMWARE_VER_LEN  8
#define FIRMWARE_VER      "0.1"

#define WEB_SITE          "https://pcb-layout.taobao.com"

typedef struct 
{
	uint16  currentCaliValue; //current calibration

	uint8   bleName[MAX_BLE_NAME_LEN + 1];  //name

	uint8   bleOn;   //turn on/off ble

	uint8   mainMenu; //main menu index

	uint8   autoDetect; //first auto detect
	
	uint8   screenLockTime; //screen lock time

	uint8   screenAngle;  //screen angle

	uint8   sampleRate;  //sample rate, counts per second

	uint8   peakValleySampleDuration; //peak & valley sample suration	
} Parameter;

typedef struct
{
	uint8  header;

	uint16 size;

	Parameter parameter;

	uint8  sum;
} StoreVector;

extern StoreVector g_storeVector;

#define g_currentCaliValue   g_storeVector.parameter.currentCaliValue
#define g_bleName            g_storeVector.parameter.bleName
#define g_bleOn              g_storeVector.parameter.bleOn
#define g_mainMenu           g_storeVector.parameter.mainMenu
#define g_autoDetect         g_storeVector.parameter.autoDetect
#define g_screenLockTime     g_storeVector.parameter.screenLockTime
#define g_screenAngle        g_storeVector.parameter.screenAngle
#define g_sampleRate         g_storeVector.parameter.sampleRate
#define g_peakValleySampleDuration  g_storeVector.parameter.peakValleySampleDuration

extern bool LoadParameter();
extern void LoadDefaultParameter();

extern bool SaveParameter();

#endif

