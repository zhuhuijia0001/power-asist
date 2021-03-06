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

#include "menuMessage.h"

#include "PowerAsist.h"

#include "npi.h"

#define DETAIL_MENU_TIMERID_REFRESH  (POWERASIST_FIRST_TIMERID + 0)

#define DETAIL_MENU_TIMERID_LOCK     (POWERASIST_FIRST_TIMERID + 1)

//key state
static uint8 s_keyLeftStatus = HAL_KEY_STATE_RELEASE;
static uint8 s_keyRightStatus = HAL_KEY_STATE_RELEASE;
static uint8 s_keyMenuStatus = HAL_KEY_STATE_RELEASE;

static void DrawDetailMenuContent()
{	
	uint8 integer;
	uint16 integer16;

	uint16 frac;

	GetDPVoltage(&integer, &frac);
	DrawDetailDp(integer, frac);
	
	GetDMVoltage(&integer, &frac);
	DrawDetailDm(integer, frac);
	
	if (GetBusVoltage(&integer, &frac))
	{
		DrawDetailVoltage(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		DrawDetailCurrent(integer, frac);
	}
	
	if (GetLoadPower(&integer, &frac))
	{
		DrawDetailPower(integer, frac);
	}

	wh_ah_status status = GetWhAndAhStatus();
	
	GetLoadWh(&integer16, &frac);
	if (status == WH_AH_STATUS_STOPPED)
	{
		DrawDetailWh(integer16, frac, RED);
	}
	else
	{
		DrawDetailWh(integer16, frac, GREEN);
	}
	
	GetLoadAh(&integer16, &frac);
	if (status == WH_AH_STATUS_STOPPED)
	{
		DrawDetailAh(integer16, frac, RED);
	}
	else
	{
		DrawDetailAh(integer16, frac, GREEN);
	}
}

static void RefreshDetailMenuContent()
{	
	uint8 integer;
	uint16 integer16;

	uint16 frac;

	GetDPVoltage(&integer, &frac);
	//TRACE("dp voltage:%d.%02dV\r\n", integer, frac);
	DrawDetailDp(integer, frac);
	
	GetDMVoltage(&integer, &frac);
	//TRACE("dm voltage:%d.%02dV\r\n", integer, frac);
	DrawDetailDm(integer, frac);

	if (GetBusVoltage(&integer, &frac))
	{
		//TRACE("bus voltage:%d.%04dV\r\n", integer, frac);

		DrawDetailVoltageDelta(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		//TRACE("current:%d.%04dA\r\n", integer, frac);

		DrawDetailCurrentDelta(integer, frac);
	}
	
	if (GetLoadPower(&integer, &frac))
	{
		//TRACE("power:%d.%04dW\r\n", integer, frac);

		DrawDetailPowerDelta(integer, frac);
	}

	wh_ah_status status = GetWhAndAhStatus();
	
	GetLoadWh(&integer16, &frac);
	if (status == WH_AH_STATUS_STOPPED)
	{
		DrawDetailWhDelta(integer16, frac, RED);
	}
	else
	{
		DrawDetailWhDelta(integer16, frac, GREEN);
	}
	
	GetLoadAh(&integer16, &frac);
	if (status == WH_AH_STATUS_STOPPED)
	{
		DrawDetailAhDelta(integer16, frac, RED);
	}
	else
	{
		DrawDetailAhDelta(integer16, frac, GREEN);
	}
}

static void DrawSnifferType()
{
	SnifferStatus sniffer = GetCurrentSnifferStatus();
	uint8 voltage = 0;

	if (sniffer != SNIFFER_NONE)
	{
		if (sniffer == SNIFFER_QC_20 || sniffer == SNIFFER_PD)
		{
			voltage = GetCurrentSnifferTargetVoltage();
		}
	}
	
	DrawDetailSniffer(sniffer, voltage);
}

static void OnMenuCreate(MENU_ID prevId)
{
	s_keyLeftStatus = HAL_KEY_STATE_RELEASE;
	s_keyRightStatus = HAL_KEY_STATE_RELEASE;
	s_keyMenuStatus = HAL_KEY_STATE_RELEASE;
		
	DrawDetailMenu();

	DrawSnifferType();
	
	DrawDetailMenuContent();

	uint32 sampleInterval = 1000ul / g_sampleRate;
	StartPowerAsistTimer(DETAIL_MENU_TIMERID_REFRESH, sampleInterval, true);

	if (g_screenLockTime != LOCK_NEVER)
	{
		uint32 lockTime = g_screenLockTime * 60 * 1000ul;
		StartPowerAsistTimer(DETAIL_MENU_TIMERID_LOCK, lockTime, false);
	}
}

static void OnMenuDestroy(MENU_ID nextId)
{
	StopPowerAsistTimer(DETAIL_MENU_TIMERID_REFRESH);

	if (g_screenLockTime != LOCK_NEVER)
	{
		StopPowerAsistTimer(DETAIL_MENU_TIMERID_LOCK);
	}
	
	ClearScreen(BLACK);
}

static const unsigned char *str_warning = "WARNING";

static const unsigned char *s_str_sniffer_warning = "High voltage\nwill appear on\nload port in\nsniffering mode";

#define MESSAGE_LEFT   0
#define MESSAGE_TOP    22

static void MessageCallbackWarning(uint8 result)
{
	if (result == MESSAGE_OK)
	{
		SwitchToMenu(MENU_ID_SNIFFER);
	}
}

static void MessageCallbackSniffer(uint8 result)
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
		
		EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, s_str_sniffer_warning, str_warning, MSG_TYPE_OK | MSG_TYPE_WARNING, MessageCallbackWarning);
	}
	else
	{
		SwitchToMenu(GetMainMenu(g_mainMenu));
	}
}

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_LEFT)
	{
	  	wh_ah_status status;
		
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			s_keyLeftStatus = HAL_KEY_STATE_PRESS;
			
			break;

		case HAL_KEY_STATE_LONG:
			s_keyLeftStatus = HAL_KEY_STATE_LONG;
			
			status = GetWhAndAhStatus();
			if (status == WH_AH_STATUS_STOPPED)
			{
				StartAccumulateWhAndAh();

				DrawDetailMenuContent();
			}
	
			break;

		case HAL_KEY_STATE_RELEASE:
			if (s_keyLeftStatus == HAL_KEY_STATE_PRESS)
			{
				TRACE("key left pressed\r\n");

				if (GetCurrentSnifferStatus() != SNIFFER_NONE)
				{
					EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, "RELEASE SNIFFING?", NULL, MSG_TYPE_YES_NO, MessageCallbackSniffer);
				}
				else
				{
					EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, s_str_sniffer_warning, str_warning, MSG_TYPE_OK | MSG_TYPE_WARNING, MessageCallbackWarning);
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
		wh_ah_status status;
		
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			s_keyRightStatus = HAL_KEY_STATE_PRESS;
			
			break;

		case HAL_KEY_STATE_LONG:
			s_keyRightStatus = HAL_KEY_STATE_LONG;

			status = GetWhAndAhStatus();
			if (status == WH_AH_STATUS_STARTED)
			{
				StopAccumulateWhAndAh();

				DrawDetailMenuContent();
			}
			else
			{
				FlushLoadWhAndAh();
			}
	
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
	if (timerId == DETAIL_MENU_TIMERID_REFRESH)
	{
		RefreshDetailMenuContent();
	}
	else if (timerId == DETAIL_MENU_TIMERID_LOCK)
	{
		SwitchToMenu(MENU_ID_LOCK);
	}
}

BEGIN_MENU_HANDLER(MENU_ID_DETAIL)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

