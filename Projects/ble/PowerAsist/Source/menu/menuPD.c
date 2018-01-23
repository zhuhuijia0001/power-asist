
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "LCD.h"
#include "GUI.h"

#include "Meter.h"

#include "UsbPd.h"

#include "menuMessage.h"

#include "Parameter.h"

#include "Global.h"

#include "powerAsistGATTprofile.h"
#include "PowerAsist.h"

#include "npi.h"

#define PD_MENU_TIMERID          (POWERASIST_FIRST_TIMERID + 0)

static uint8 s_curCapIndex = 0;

static const CapabilityStruct *s_caps = NULL;
static uint8 s_capCnt = 0;

static bool s_requesting = false;

void SetPDCapIndex(uint8 index)
{
	s_curCapIndex = index;
}

static void RequestCallback(uint8 requestedVoltage, uint8 res)
{
	if (GetCurrentMenu() != MENU_ID_PD)
	{
		return;
	}
	
	if (res == REQUEST_ACCEPT)
	{
		TRACE("request accept\r\n");

		SetCurrentSnifferTargetVoltage(requestedVoltage);
		
		DrawPDTipAccept();
	}
	else if (res == REQUEST_PS_RDY)
	{
		//DrawPDTipPsRdy();
	}
	else if (res == REQUEST_REJECT)
	{
		TRACE("request rejected\r\n");

		DrawPDTipReject();
	}
	else if (res == REQUEST_FAILED)
	{
		TRACE("request failed\r\n");

		DrawPDTipFailed();
	}

	s_requesting = false;
}

static void DrawPDMenuContent()
{	
	uint8 integer;
	uint16 frac;
	
	if (GetBusVoltage(&integer, &frac))
	{
		DrawPDVoltage(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		DrawPDCurrent(integer, frac);
	}
}

static void RefreshPDMenuContent()
{	
	uint8 integer;
	uint16 frac;
	
	if (GetBusVoltage(&integer, &frac))
	{
		DrawPDVoltageDelta(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		DrawPDCurrentDelta(integer, frac);
	}
}

static void OnMenuCreate(MENU_ID prevId)
{
	const CapabilityList *list = GetPDCaps();
	s_caps = list->caps;
	s_capCnt = list->cnt;

	DrawPDMenu();
	DrawPDItem(s_curCapIndex, &s_caps[s_curCapIndex]);

	DrawPDMenuContent();

	uint32 sampleInterval = 1000ul / g_sampleRate;
	StartPowerAsistTimer(PD_MENU_TIMERID, sampleInterval, true);
	
	PdRequest(s_curCapIndex + 1, RequestCallback);
	s_requesting = true;
}

static void OnMenuDestroy(MENU_ID nextId)
{
	StopPowerAsistTimer(PD_MENU_TIMERID);
	
	ClearScreen(BLACK);
}

static void MessageCallback(uint8 result)
{
	if (result == MESSAGE_YES)
	{
		ExitUsbPd();

		SetCurrentSnifferStatus(SNIFFER_NONE);
	}
	
	SwitchToMenu(GetMainMenu(g_mainMenu));
}

#define MESSAGE_LEFT   0
#define MESSAGE_TOP    30

static void OnMenuKey(uint8 key, uint8 type)
{
	if (s_requesting)
	{
		return;
	}
	
	if (key == KEY_LEFT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_capCnt > 0)
			{
				s_curCapIndex += s_capCnt - 1;
				s_curCapIndex %= s_capCnt;

				DrawPDItem(s_curCapIndex, &s_caps[s_curCapIndex]);

				ClearPDTip();
				PdRequest(s_curCapIndex + 1, RequestCallback);
				s_requesting = true;
			}
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (GetCurrentSnifferStatus() != SNIFFER_NONE)
			{
				EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, "RELEASE SNIFFING?", NULL, MSG_TYPE_YES_NO, MessageCallback);
			}
			else
			{
				ExitUsbPd();
				SwitchToMenu(GetMainMenu(g_mainMenu));
			}
	
			break;
		}
	}
	else if (key == KEY_RIGHT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_capCnt > 0)
			{
				s_curCapIndex++;
				s_curCapIndex %= s_capCnt;

				DrawPDItem(s_curCapIndex, &s_caps[s_curCapIndex]);

				ClearPDTip();
				PdRequest(s_curCapIndex + 1, RequestCallback);
				s_requesting = true;
			}
			
			break;
		}
	}
}

static void OnMenuTimeout(uint16 timerId)
{
	RefreshPDMenuContent();
}

BEGIN_MENU_HANDLER(MENU_ID_PD)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

