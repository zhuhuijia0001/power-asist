
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

static void OnMenuCreate(MENU_ID prevId)
{
	EnableLcdBacklight(false);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	EnableLcdBacklight(true);
}

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_MENU)
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
}

BEGIN_MENU_HANDLER(MENU_ID_SLEEP)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

