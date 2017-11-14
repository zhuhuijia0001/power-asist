#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "LCD.h"
#include "GUI.h"

#include "Meter.h"

#include "Global.h"

#include "Parameter.h"

#include "PowerAsist.h"

//welcome timer id
#define WELCOME_MENU_TIMERID_EXPIRED      (POWERASIST_FIRST_TIMERID + 0)

//delay time (ms)
#define WELCOME_MENU_DELAY_TIME            1200

//picture timer id
#define PICTURE_TIMERID                   (POWERASIST_FIRST_TIMERID + 1)
#define PICTURE_DELAY_TIME                 20
#define PICTURE_STEP_WIDTH                 3

static uint16 s_logoX = 0;
static uint16 s_logoY = 0;

static uint16 s_logoOffsetx = 0;

static void OnMenuCreate(MENU_ID prevId)
{
	EnableLcdBacklight(true);
	
	//start accumulate Wh & Ah
	StartAccumulateWhAndAh();
	
	s_logoX = (GetScreenWidth() - g_Logo.width) / 2;
	s_logoY = (GetScreenHeight() - g_Logo.height) / 3;
	s_logoOffsetx = 0;

	StartPowerAsistTimer(PICTURE_TIMERID, PICTURE_DELAY_TIME, true);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	ClearScreen(BLACK);
}

static void OnMenuTimeout(uint16 timerId)
{
	if (timerId == PICTURE_TIMERID)
	{
		uint16 width;
		if (s_logoOffsetx + PICTURE_STEP_WIDTH <= g_Logo.width)
		{
			width = PICTURE_STEP_WIDTH;
		}
		else
		{
			width = g_Logo.width - s_logoOffsetx;
		}

		DrawPicturePart(s_logoX, s_logoY, s_logoOffsetx, 0, width, g_Logo.height, &g_Logo);
		s_logoOffsetx += width;
		if (s_logoOffsetx == g_Logo.width)
		{
			StopPowerAsistTimer(PICTURE_TIMERID);

			StartPowerAsistTimer(WELCOME_MENU_TIMERID_EXPIRED, WELCOME_MENU_DELAY_TIME, false);
		}
	}
	else if (timerId == WELCOME_MENU_TIMERID_EXPIRED)
	{
		if (g_autoDetect)
		{
			SwitchToMenu(MENU_ID_AUTO);
		}
		else
		{
			if (GetMainMenu(g_mainMenu) == MENU_ID_SLEEP)
			{
				g_mainMenu = GetPrevMainMenuIndex(g_mainMenu);

				SaveParameter();
			}

			SwitchToMenu(GetMainMenu(g_mainMenu));
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_WELCOME)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

