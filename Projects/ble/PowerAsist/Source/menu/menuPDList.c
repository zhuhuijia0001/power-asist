
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "LCD.h"
#include "GUI.h"

#include "Meter.h"
#include "UsbPd.h"

#include "Parameter.h"

#include "Global.h"

#include "powerAsistGATTprofile.h"
#include "PowerAsist.h"

#include "npi.h"

static const CapabilityStruct *s_caps = NULL;
static uint8 s_capCnt = 0;

static uint8 s_curIndex = 0;

static bool s_capsGot = false;

static void CapsCallback(uint8 res, const CapabilityList *list)
{
	if (GetCurrentMenu() != MENU_ID_PD_LIST)
	{
		return;
	}
	
	if (res == GET_CAPS_OK)
	{
		s_caps = list->caps;
		s_capCnt = list->cnt;

		s_curIndex = 0;

		SetCurrentSnifferStatus(SNIFFER_PD);
		SetCurrentSnifferTargetVoltage(s_caps[s_curIndex].voltage / 1000ul);
		
		DrawPDCapabilityList(s_caps, s_capCnt);

		DrawPDSelCapability(s_curIndex, &s_caps[s_curIndex]);
	}
	else if (res == GET_CAPS_FAILED)
	{
		TRACE("get caps failed\r\n");

		DrawPDListFailed();
	}

	s_capsGot = true;
}

static wh_ah_status s_savedStatus;

static void OnMenuCreate(MENU_ID prevId)
{
	s_caps = NULL;
	s_capCnt = 0;

	s_capsGot = false;

	//stop accumulate
	s_savedStatus = GetWhAndAhStatus();
	if (s_savedStatus == WH_AH_STATUS_STARTED)
	{
		StopAccumulateWhAndAh();
	}
	
	DrawPDListMenu();

	EnterUsbPd(CapsCallback);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	ClearScreen(BLACK);

	//Start accumulate
	if (s_savedStatus == WH_AH_STATUS_STARTED)
	{
		StartAccumulateWhAndAh();
	}
}

extern void SetPDCapIndex(uint8 index);

static void OnMenuKey(uint8 key, uint8 type)
{
	if (!s_capsGot)
	{
		return;
	}
	
	if (key == KEY_LEFT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_capCnt > 0)
			{
				DrawPDNormalCapability(s_curIndex, &s_caps[s_curIndex]);

				s_curIndex += s_capCnt - 1;
				s_curIndex %= s_capCnt;

				DrawPDSelCapability(s_curIndex, &s_caps[s_curIndex]);
			}
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_capCnt > 0)
			{
				SetPDCapIndex(s_curIndex);
							
				SwitchToMenu(MENU_ID_PD);
			}
			else
			{
				//get capability failed
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
		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_capCnt > 0)
			{
				DrawPDNormalCapability(s_curIndex, &s_caps[s_curIndex]);

				s_curIndex++;
				s_curIndex %= s_capCnt;

				DrawPDSelCapability(s_curIndex, &s_caps[s_curIndex]);
			}
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_PD_LIST)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

