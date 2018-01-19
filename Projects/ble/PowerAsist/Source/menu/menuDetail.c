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

#define DETAIL_MENU_TIMERID          (POWERASIST_FIRST_TIMERID + 0)

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

	GetLoadWh(&integer16, &frac);
	DrawDetailWh(integer16, frac);

	GetLoadAh(&integer16, &frac);
	DrawDetailAh(integer16, frac);
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

	GetLoadWh(&integer16, &frac);
	//TRACE("Wh:%d.%04dWh\r\n", integer16, frac);
	DrawDetailWh(integer16, frac);

	GetLoadAh(&integer16, &frac);
	//TRACE("Ah:%d.%04dAh\r\n", integer16, frac);
	DrawDetailAh(integer16, frac);
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
	StartPowerAsistTimer(DETAIL_MENU_TIMERID, sampleInterval, true);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	StopPowerAsistTimer(DETAIL_MENU_TIMERID);

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
			
			DrawDetailMenu();
			DrawSnifferType();
			DrawDetailMenuContent();
	
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
			
			DrawDetailMenu();
			DrawSnifferType();
			DrawDetailMenuContent();
	
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
	RefreshDetailMenuContent();
}

BEGIN_MENU_HANDLER(MENU_ID_DETAIL)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

