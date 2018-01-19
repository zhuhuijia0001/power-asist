#include "bcomdef.h"

#include "menuOp.h"
#include "menu.h"

#include "Global.h"

static const MENU_ID s_mainMenus[] = 
{
	MENU_ID_MAIN,

	MENU_ID_DETAIL,

	MENU_ID_SLEEP,
};

static SnifferStatus s_snifferStatus = SNIFFER_NONE;

static uint8 s_snifferVoltage = 0;

SnifferStatus GetCurrentSnifferStatus()
{
	return s_snifferStatus;
}

void SetCurrentSnifferStatus(SnifferStatus status)
{
	s_snifferStatus = status;
}

uint8 GetCurrentSnifferTargetVoltage()
{
	return s_snifferVoltage;
}

void SetCurrentSnifferTargetVoltage(uint8 voltage)
{
	s_snifferVoltage = voltage;
}

MENU_ID GetMainMenu(uint8 index)
{
	return s_mainMenus[index];
}

uint8 GetNextMainMenuIndex(uint8 curIndex)
{
	curIndex++;
	curIndex %= sizeof(s_mainMenus) / sizeof(s_mainMenus[0]);

	return curIndex;
}

uint8 GetPrevMainMenuIndex(uint8 curIndex)
{
	curIndex += sizeof(s_mainMenus) / sizeof(s_mainMenus[0]) - 1;
	curIndex %= sizeof(s_mainMenus) / sizeof(s_mainMenus[0]);

	return curIndex;
}

