
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "GUI.h"

static MENU_ID s_prevId = MENU_ID_NONE;

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevId = prevId;
	
	DrawVersionMenu();
}

static void OnMenuDestroy(MENU_ID nextId)
{
	ClearScreen(BLACK);
}

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			SwitchToMenu(s_prevId);

			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_VERSION)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

