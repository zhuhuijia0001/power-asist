#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "LCD.h"
#include "GUI.h"

#include "Meter.h"

#include "QuickCharge.h"

#include "Global.h"

#include "Parameter.h"

#include "powerAsistGATTprofile.h"
#include "PowerAsist.h"

#include "UsbPd.h"

#include "menuMessage.h"

#include "npi.h"

#define MAIN_MENU_TIMERID_MEASURE      (POWERASIST_FIRST_TIMERID + 0)

#define MAIN_MENU_TIMERID_RTC          (POWERASIST_FIRST_TIMERID + 1)

#define MAIN_RTC_INTERVAL          1000ul

//key state
static uint8 s_keyLeftStatus = HAL_KEY_STATE_RELEASE;
static uint8 s_keyRightStatus = HAL_KEY_STATE_RELEASE;
static uint8 s_keyMenuStatus = HAL_KEY_STATE_RELEASE;

static void DrawMainMenuMeasure()
{
	uint8 integer;
	uint16 frac;
	
	if (GetBusVoltage(&integer, &frac))
	{
		DrawMainVoltage(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		DrawMainCurrent(integer, frac);
	}
	
	if (GetLoadPower(&integer, &frac))
	{
		DrawMainPower(integer, frac);
	}
}

static void RefreshMainMenuMeasure()
{		
	uint8 integer;
	uint16 frac;
	
	if (GetBusVoltage(&integer, &frac))
	{
		//TRACE("bus voltage:%d.%04dV\r\n", integer, frac);

		DrawMainVoltageDelta(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		//TRACE("current:%d.%04dA\r\n", integer, frac);

		DrawMainCurrentDelta(integer, frac);
	}
	
	if (GetLoadPower(&integer, &frac))
	{
		//TRACE("power:%d.%04dW\r\n", integer, frac);

		DrawMainPowerDelta(integer, frac);
	}
}

static void DrawMainMenuRTC()
{
	TimeStruct time;
	if (GetRTCTime(&time))
	{
		DrawMainDateTime(&time);
	}
}

static void RefreshMainMenuRTC()
{
	TimeStruct time;
	if (GetRTCTime(&time))
	{
		DrawMainDateTimeDelta(&time);
	}
}

static void OnMenuCreate(MENU_ID prevId)
{
	s_keyLeftStatus = HAL_KEY_STATE_RELEASE;
	s_keyRightStatus = HAL_KEY_STATE_RELEASE;
	s_keyMenuStatus = HAL_KEY_STATE_RELEASE;
	
	DrawMainMenu();

	DrawMainMenuMeasure();
	DrawMainMenuRTC();

	uint32 sampleInterval = 1000ul / g_sampleRate;
	StartPowerAsistTimer(MAIN_MENU_TIMERID_MEASURE, sampleInterval, true);

	StartPowerAsistTimer(MAIN_MENU_TIMERID_RTC, MAIN_RTC_INTERVAL, true);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	StopPowerAsistTimer(MAIN_MENU_TIMERID_MEASURE);

	StopPowerAsistTimer(MAIN_MENU_TIMERID_RTC);

	ClearScreen(BLACK);
}

static void MessageCallback(uint8 result)
{
	if (result == MESSAGE_YES)
	{
		SnifferStatus status = GetCurrentSnifferStatus();
		if (status == SNIFFER_QC_20
			|| status == SNIFFER_QC_30)
		{
			StopQC20Sniffer();
		}
		else if (status == SNIFFER_PD)
		{
			ExitUsbPd();
		}

		SetCurrentSnifferStatus(SNIFFER_NONE);
		
		SwitchToMenu(MENU_ID_SNIFFER);
	}
	else
	{
		SwitchToMenu(GetMainMenu(g_mainMenu));
	}
}

#define MESSAGE_LEFT   0
#define MESSAGE_TOP    30

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_LEFT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			s_keyLeftStatus = HAL_KEY_STATE_PRESS;
			
			break;

		case HAL_KEY_STATE_LONG:
			s_keyLeftStatus = HAL_KEY_STATE_LONG;

			//rotate
			ClearScreen(BACKGROUND_COLOR);

			g_screenAngle += ORIENTATION_COUNT - 1;
			g_screenAngle %= ORIENTATION_COUNT;
			SetLcdOrientation((Orientation)g_screenAngle);

			SaveParameter();
			
			DrawMainMenu();

			DrawMainMenuMeasure();
			DrawMainMenuRTC();
	
			break;

		case HAL_KEY_STATE_RELEASE:
			if (s_keyLeftStatus == HAL_KEY_STATE_PRESS)
			{
				TRACE("key left pressed\r\n");

				if (GetCurrentSnifferStatus() != SNIFFER_NONE)
				{
					EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, "RELEASE SNIFFING?", MessageCallback);
				}
				else
				{
					SwitchToMenu(MENU_ID_SNIFFER);
				}
			}

			s_keyLeftStatus = HAL_KEY_STATE_RELEASE;
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			s_keyMenuStatus = HAL_KEY_STATE_PRESS;

			break;
			
		case HAL_KEY_STATE_LONG:
			s_keyMenuStatus = HAL_KEY_STATE_LONG;

			SwitchToMenu(MENU_ID_BLE_COMM);
			
			break;

		case HAL_KEY_STATE_RELEASE:
			if (s_keyMenuStatus == HAL_KEY_STATE_PRESS)
			{
				TRACE("key menu pressed\r\n");
		
				g_mainMenu = GetNextMainMenuIndex(g_mainMenu);
				SwitchToMenu(GetMainMenu(g_mainMenu));
				
				SaveParameter();
			}

			s_keyMenuStatus = HAL_KEY_STATE_RELEASE;
			
			break;
		}
	}
	else if (key == KEY_RIGHT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			s_keyRightStatus = HAL_KEY_STATE_PRESS;
			
			break;

		case HAL_KEY_STATE_LONG:
			s_keyRightStatus = HAL_KEY_STATE_LONG;

			//rotate
			ClearScreen(BACKGROUND_COLOR);

			g_screenAngle++;
			g_screenAngle %= ORIENTATION_COUNT;
			SetLcdOrientation((Orientation)g_screenAngle);

			SaveParameter();
			
			DrawMainMenu();

			DrawMainMenuMeasure();
			DrawMainMenuRTC();
	
			break;

		case HAL_KEY_STATE_RELEASE:
			if (s_keyRightStatus == HAL_KEY_STATE_PRESS)
			{
				TRACE("key right pressed\r\n");
				
				SwitchToMenu(MENU_ID_SETTING);
			}

			s_keyRightStatus = HAL_KEY_STATE_RELEASE;
			
			break;
		}
	}
}

static void OnMenuTimeout(uint16 timerId)
{
	if (timerId == MAIN_MENU_TIMERID_MEASURE)
	{
		RefreshMainMenuMeasure();
	}
	else if (timerId == MAIN_MENU_TIMERID_RTC)
	{
		RefreshMainMenuRTC();
	}
}

BEGIN_MENU_HANDLER(MENU_ID_MAIN)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

