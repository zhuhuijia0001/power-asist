
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "Parameter.h"

#include "PowerAsist.h"

#include "draw.h"
#include "GUI.h"

#include "Meter.h"

#include "Calibration.h"

#include "npi.h"

#define  VOLTAGE_CALI_TIMER_ID          (POWERASIST_FIRST_TIMERID + 0)

typedef enum
{
	Voltage_Cali_Item_Start = 0,
	Voltage_Cali_Item_OK = Voltage_Cali_Item_Start,
	Voltage_Cali_Item_Done = Voltage_Cali_Item_Start,
	
	Voltage_Cali_Item_Cancel,

	Voltage_Cali_Item_Count,

	Voltage_Cali_Item_None
} VoltageCaliItem;
static VoltageCaliItem s_curSelVoltageCaliItem = Voltage_Cali_Item_Start;

static void (*const s_drawVoltageCaliNormalItemFun[])() = 
{
	[Voltage_Cali_Item_Start] = DrawVoltageCaliNormalStart,
	[Voltage_Cali_Item_Cancel] = DrawVoltageCaliNormalCancel,
};

static void (*const s_drawVoltageCaliSelItemFun[])() = 
{
	[Voltage_Cali_Item_Start] = DrawVoltageCaliSelStart,
	[Voltage_Cali_Item_Cancel] = DrawVoltageCaliSelCancel,
};

//calibration status
typedef enum
{
	status_calibration_ready = 0, //ready for calibration
	status_calibrating,           //calibrating
	status_calibration_finished,  //finished
	
} VoltageCaliStatus;

static VoltageCaliStatus s_caliStatus = status_calibration_ready;

//calibration value
static CalibrationItem  s_voltageCaliItem[VOLTAGE_CALI_ITEM_COUNT];
static uint8            s_voltageCaliItemIndex = 0;

#define VOLTAGE_REFL_DEC     5ul
#define VOLTAGE_REFL_FRAC    0

#define VOLTAGE_REFM_DEC     12ul
#define VOLTAGE_REFM_FRAC    0

#define VOLTAGE_REFH_DEC     24ul
#define VOLTAGE_REFH_FRAC    0

static uint8 s_prevPercent = 0;

static CalibrationValue s_voltageCaliValue[] = 
{
	[0] = 
	{
		.dec = VOLTAGE_REFL_DEC,
		.frac = VOLTAGE_REFL_FRAC,
	},
	
	[1] = 
	{
		.dec = VOLTAGE_REFM_DEC,
		.frac = VOLTAGE_REFM_FRAC,
	},
	
	[2] = 
	{
		.dec = VOLTAGE_REFH_DEC,
		.frac = VOLTAGE_REFH_FRAC,
	},
};

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void CaliStepCallback(uint8 percent)
{
	TRACE("cali percentage:%d\r\n", percent);

	DrawVoltageCaliProgressStep(s_prevPercent, percent);
	s_prevPercent = percent;
}

static void CaliFinishCallback(uint16 caliAdc)
{
	TRACE("cali finished,adc:0x%04X\r\n", caliAdc);

	s_voltageCaliItem[s_voltageCaliItemIndex].adc = caliAdc;
	s_voltageCaliItem[s_voltageCaliItemIndex].val.dec = s_voltageCaliValue[s_voltageCaliItemIndex].dec;
	s_voltageCaliItem[s_voltageCaliItemIndex].val.frac = s_voltageCaliValue[s_voltageCaliItemIndex].frac;

	s_caliStatus = status_calibration_finished;

	s_voltageCaliItemIndex++;
	if (s_voltageCaliItemIndex < VOLTAGE_CALI_ITEM_COUNT)
	{
		DrawVoltageCaliSelOK();
	}
	else
	{
		//finished
		DrawVoltageCaliSelDone();
	}
}

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;

	SetCalibrationCallback(CaliStepCallback, CaliFinishCallback);
	s_voltageCaliItemIndex = 0;
	
	DrawVoltageCaliMenu();

	s_curSelVoltageCaliItem = Voltage_Cali_Item_Start;
	s_drawVoltageCaliSelItemFun[s_curSelVoltageCaliItem]();

	s_caliStatus = status_calibration_ready;

	s_prevPercent = 0;
	
	DrawVoltageCaliRef(s_voltageCaliValue[s_voltageCaliItemIndex].dec,
						s_voltageCaliValue[s_voltageCaliItemIndex].frac);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	ClearScreen(BLACK);
}

static void OnMenuKey(uint8 key, uint8 type)
{           
	if (key == KEY_LEFT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_caliStatus == status_calibration_ready)
			{
				s_drawVoltageCaliNormalItemFun[s_curSelVoltageCaliItem]();
				s_curSelVoltageCaliItem += Voltage_Cali_Item_Count - 1;
				s_curSelVoltageCaliItem %= Voltage_Cali_Item_Count;
				s_drawVoltageCaliSelItemFun[s_curSelVoltageCaliItem]();
			}

			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		if (type == HAL_KEY_STATE_PRESS)
		{
			if (s_caliStatus == status_calibration_ready)
			{
				if (s_curSelVoltageCaliItem == Voltage_Cali_Item_Start)
				{
					//Clear process region
					ClearVoltageCalProcess();
					
					DrawVoltageCaliProgressBar();
					
					StartCalibration(VOLTAGE_CALI_TIMER_ID, GetBusVoltageAdcValue);
					s_prevPercent = 0;
					
					s_caliStatus = status_calibrating;
				}
				else if (s_curSelVoltageCaliItem == Voltage_Cali_Item_Cancel)
				{
					SwitchToMenu(s_prevMenuId);
				}
			}
			else if (s_caliStatus == status_calibration_finished)
			{
				if (s_voltageCaliItemIndex < VOLTAGE_CALI_ITEM_COUNT)
				{
					//Clear process region
					ClearVoltageCalProcess();

					DrawVoltageCaliRef(s_voltageCaliValue[s_voltageCaliItemIndex].dec,
						s_voltageCaliValue[s_voltageCaliItemIndex].frac);

					//draw process tips
					s_curSelVoltageCaliItem = Voltage_Cali_Item_Start;
					DrawVoltageCaliSelStart();
					DrawVoltageCaliNormalCancel();

					s_caliStatus = status_calibration_ready;
				}
				else
				{
					//finished
					for (uint8 i = 0; i < VOLTAGE_CALI_ITEM_COUNT; i++)
					{
						g_voltageCaliItem[i] = s_voltageCaliItem[i];
					}
					
					g_voltageCaliItemCount = s_voltageCaliItemIndex;
					
					SaveCalibration();
					
					SwitchToMenu(s_prevMenuId);
				}
			}
		}
	}
	else if (key == KEY_RIGHT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_caliStatus == status_calibration_ready)
			{
				s_drawVoltageCaliNormalItemFun[s_curSelVoltageCaliItem]();
				s_curSelVoltageCaliItem++;
				s_curSelVoltageCaliItem %= Voltage_Cali_Item_Count;
				s_drawVoltageCaliSelItemFun[s_curSelVoltageCaliItem]();
			}
			
			break;
		}
	}
}

static void OnMenuTimeout(uint16 timerId)
{
	CalibrationTimerProcess();
}

BEGIN_MENU_HANDLER(MENU_ID_VOLTAGE_CALI)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

