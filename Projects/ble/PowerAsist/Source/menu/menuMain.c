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

static uint8 s_keyMenuStatus = HAL_KEY_STATE_RELEASE;

static void RefreshMainMenuMeasure()
{		
	uint8 integer;
	uint16 frac;
	
	/*
	if (GetShuntVoltage(&integer, &frac))
	{
		TRACE("shunt voltage:%d.%04dmV\r\n", integer, frac);
	}
	*/
	
	if (GetBusVoltage(&integer, &frac))
	{
		//TRACE("bus voltage:%d.%04dV\r\n", integer, frac);

		DrawMainVoltage(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		//TRACE("current:%d.%04dA\r\n", integer, frac);

		DrawMainCurrent(integer, frac);
	}
	
	if (GetLoadPower(&integer, &frac))
	{
		//TRACE("power:%d.%04dW\r\n", integer, frac);

		DrawMainPower(integer, frac);
	}
	
	GetDPVoltage(&integer, &frac);
	//TRACE("dp voltage:%d.%02dV\r\n", integer, frac);
	
	GetDMVoltage(&integer, &frac);
	//TRACE("dm voltage:%d.%02dV\r\n", integer, frac);
}

static void RefreshMainMenuRTC()
{
	TimeStruct time;
	if (GetRTCTime(&time))
	{
		DrawDateTime(&time);
	}
}

static void OnMenuCreate(MENU_ID prevId)
{
	s_keyMenuStatus = HAL_KEY_STATE_RELEASE;
	
	DrawMainMenu();

	RefreshMainMenuMeasure();
	RefreshMainMenuRTC();

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
			TRACE("key left pressed\r\n");

			if (GetCurrentSnifferStatus() != SNIFFER_NONE)
			{
				EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, "RELEASE SNIFFING?", MessageCallback);
			}
			else
			{
				SwitchToMenu(MENU_ID_SNIFFER);
			}
			
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
			TRACE("key right pressed\r\n");

			SwitchToMenu(MENU_ID_SETTING);
			
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

