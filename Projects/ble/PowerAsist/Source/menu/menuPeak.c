
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "LCD.h"
#include "GUI.h"

#include "Global.h"
#include "Parameter.h"

#include "powerAsistGATTprofile.h"
#include "PowerAsist.h"

#include "QuickCharge.h"
#include "UsbPd.h"

#include "menuMessage.h"

static void OnMenuCreate(MENU_ID prevId)
{

}

static void OnMenuDestroy(MENU_ID nextId)
{
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
			SwitchToMenu(MENU_ID_SETTING);
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_PEAK)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()


