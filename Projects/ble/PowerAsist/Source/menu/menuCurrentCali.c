
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

#define  CURRENT_CALI_TIMER_ID          (POWERASIST_FIRST_TIMERID + 0)

typedef enum
{
	Current_Cali_Item_Start = 0,
	Current_Cali_Item_OK = Current_Cali_Item_Start,
	Current_Cali_Item_Done = Current_Cali_Item_Start,
	
	Current_Cali_Item_Cancel,

	Current_Cali_Item_Count,

	Current_Cali_Item_None
} CurrentCaliItem;
static CurrentCaliItem s_curSelCurrentCaliItem = Current_Cali_Item_Start;

static void (*const s_drawCurrentCaliNormalItemFun[])() = 
{
	[Current_Cali_Item_Start] = DrawCurrentCaliNormalStart,
	[Current_Cali_Item_Cancel] = DrawCurrentCaliNormalCancel,
};

static void (*const s_drawCurrentCaliSelItemFun[])() = 
{
	[Current_Cali_Item_Start] = DrawCurrentCaliSelStart,
	[Current_Cali_Item_Cancel] = DrawCurrentCaliSelCancel,
};

//calibration status
typedef enum
{
	status_calibration_ready = 0, //ready for calibration
	status_calibrating,           //calibrating
	status_calibration_finished,  //finished
	
} CurrentCaliStatus;

static CurrentCaliStatus s_caliStatus = status_calibration_ready;

//calibration value
static CalibrationItem  s_currentCaliItem[CURRENT_CALI_ITEM_COUNT];
static uint8            s_currentCaliItemIndex = 0;

#define CURRENT_ZERO_DEC     0
#define CURRENT_ZERO_FRAC    0

#define CURRENT_REFL_DEC     0
#define CURRENT_REFL_FRAC    1000

#define CURRENT_REFM_DEC     1ul
#define CURRENT_REFM_FRAC    0

#define CURRENT_REFH_DEC     3ul
#define CURRENT_REFH_FRAC    0

static uint8 s_prevPercent = 0;

static CalibrationValue s_currentCaliValue[] = 
{
	[0] = 
	{
		.dec = CURRENT_ZERO_DEC,
		.frac = CURRENT_ZERO_FRAC,
	},
	
	[1] = 
	{
		.dec = CURRENT_REFL_DEC,
		.frac = CURRENT_REFL_FRAC,
	},
	
	[2] = 
	{
		.dec = CURRENT_REFM_DEC,
		.frac = CURRENT_REFM_FRAC,
	},

	[3] = 
	{
		.dec = CURRENT_REFH_DEC,
		.frac = CURRENT_REFH_FRAC,
	},
};


static void CaliStepCallback(uint8 percent)
{
	TRACE("cali percentage:%d\r\n", percent);

	DrawCurrentCaliProgressStep(s_prevPercent, percent);
	s_prevPercent = percent;
}

static void CaliFinishCallback(uint16 caliAdc)
{
	TRACE("cali finished,adc:0x%04X\r\n", caliAdc);

	s_currentCaliItem[s_currentCaliItemIndex].adc = caliAdc;
	s_currentCaliItem[s_currentCaliItemIndex].val.dec = s_currentCaliValue[s_currentCaliItemIndex].dec;
	s_currentCaliItem[s_currentCaliItemIndex].val.frac = s_currentCaliValue[s_currentCaliItemIndex].frac;

	s_caliStatus = status_calibration_finished;

	s_currentCaliItemIndex++;
	if (s_currentCaliItemIndex < CURRENT_CALI_ITEM_COUNT)
	{
		DrawCurrentCaliSelOK();
	}
	else
	{
		//finished
		DrawCurrentCaliSelDone();
	}
}


static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;

	SetCalibrationCallback(CaliStepCallback, CaliFinishCallback);
	s_currentCaliItemIndex = 0;
	
	DrawCurrentCaliMenu();

	s_curSelCurrentCaliItem = Current_Cali_Item_Start;
	s_drawCurrentCaliSelItemFun[s_curSelCurrentCaliItem]();

	s_caliStatus = status_calibration_ready;

	s_prevPercent = 0;
	
	DrawCurrentCaliRef(s_currentCaliValue[s_currentCaliItemIndex].dec,
						s_currentCaliValue[s_currentCaliItemIndex].frac);
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
				s_drawCurrentCaliNormalItemFun[s_curSelCurrentCaliItem]();
				s_curSelCurrentCaliItem += Current_Cali_Item_Count - 1;
				s_curSelCurrentCaliItem %= Current_Cali_Item_Count;
				s_drawCurrentCaliSelItemFun[s_curSelCurrentCaliItem]();
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
				if (s_curSelCurrentCaliItem == Current_Cali_Item_Start)
				{
					//Clear process region
					ClearCurrentCalProcess();
					
					DrawCurrentCaliProgressBar();
					
					StartCalibration(CURRENT_CALI_TIMER_ID, GetLoadCurrentAdcValue);
					s_prevPercent = 0;
					
					s_caliStatus = status_calibrating;
				}
				else if (s_curSelCurrentCaliItem == Current_Cali_Item_Cancel)
				{
					SwitchToMenu(s_prevMenuId);
				}
			}
			else if (s_caliStatus == status_calibration_finished)
			{
				if (s_currentCaliItemIndex < CURRENT_CALI_ITEM_COUNT)
				{
					//Clear process region
					ClearCurrentCalProcess();

					DrawCurrentCaliRef(s_currentCaliValue[s_currentCaliItemIndex].dec,
						s_currentCaliValue[s_currentCaliItemIndex].frac);

					//draw process tips
					s_curSelCurrentCaliItem = Current_Cali_Item_Start;
					DrawCurrentCaliSelStart();
					DrawCurrentCaliNormalCancel();

					s_caliStatus = status_calibration_ready;
				}
				else
				{
					//finished
					for (uint8 i = 0; i < CURRENT_CALI_ITEM_COUNT; i++)
					{
						g_currentCaliItem[i] = s_currentCaliItem[i];
					}
					
					g_currentCaliItemCount = s_currentCaliItemIndex;
					
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
				s_drawCurrentCaliNormalItemFun[s_curSelCurrentCaliItem]();
				s_curSelCurrentCaliItem++;
				s_curSelCurrentCaliItem %= Current_Cali_Item_Count;
				s_drawCurrentCaliSelItemFun[s_curSelCurrentCaliItem]();
			}

			break;
		}
	}
}

static void OnMenuTimeout(uint16 timerId)
{
	CalibrationTimerProcess();
}

BEGIN_MENU_HANDLER(MENU_ID_CURRENT_CALI)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()



