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
#define DETAIL_REFRESH_INTERVAL      200

static void RefreshDetailMenuContent()
{	
	uint8 integer;
	uint16 integer16;

	uint8 frac8;
	
	uint16 frac;

	GetDPVoltage(&integer, &frac);
	//TRACE("dp voltage:%d.%02dV\r\n", integer, frac);
	DrawDetailDp(integer, frac);
	
	GetDMVoltage(&integer, &frac);
	//TRACE("dm voltage:%d.%02dV\r\n", integer, frac);
	DrawDetailDm(integer, frac);

	if (GetADCTemperature(&integer, &frac8))
	{
		TRACE("temp:%d.%01d degree\r\n", integer, frac8);
	}
	
	if (GetBusVoltage(&integer, &frac))
	{
		//TRACE("bus voltage:%d.%04dV\r\n", integer, frac);

		DrawDetailVoltage(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		//TRACE("current:%d.%04dA\r\n", integer, frac);

		DrawDetailCurrent(integer, frac);
	}
	
	if (GetLoadPower(&integer, &frac))
	{
		//TRACE("power:%d.%04dW\r\n", integer, frac);

		DrawDetailPower(integer, frac);
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
	uint8 volt = 0;
		
	uint8 integer;

	if (sniffer != SNIFFER_NONE)
	{
		GetBusVoltage(&integer, NULL);

		if (integer >= 4 && integer <= 6)
		{
			volt = 5;
		}
		else if (integer >= 8 && integer <= 10)
		{
			volt = 9;
		}
		else if (integer >= 11 && integer <= 13)
		{
			volt = 12;
		}
		else if (integer >= 14 && integer <= 16)
		{
			volt = 15;
		}
		else if (integer >= 19 && integer <= 21)
		{
			volt = 20;
		}
		else
		{
			volt = integer;
		}
	}
	
	DrawDetailSniffer(sniffer, volt);
}

static void OnMenuCreate(MENU_ID prevId)
{
	DrawDetailMenu();

	DrawSnifferType();
	
	RefreshDetailMenuContent();
	
	StartPowerAsistTimer(DETAIL_MENU_TIMERID, DETAIL_REFRESH_INTERVAL, true);
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
			TRACE("key menu pressed\r\n");

			g_mainMenu = GetNextMainMenuIndex(g_mainMenu);
			SwitchToMenu(GetMainMenu(g_mainMenu));
			
			SaveParameter();
			
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
	RefreshDetailMenuContent();
}

BEGIN_MENU_HANDLER(MENU_ID_DETAIL)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

