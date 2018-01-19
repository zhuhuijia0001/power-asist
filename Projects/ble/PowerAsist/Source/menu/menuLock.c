
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

#include "npi.h"

static uint8 s_keyMenuStatus = HAL_KEY_STATE_RELEASE;

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void OnMenuCreate(MENU_ID prevId)
{
	s_keyMenuStatus = HAL_KEY_STATE_RELEASE;

	s_prevMenuId = prevId;
	
	EnableLcdBacklight(false);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	EnableLcdBacklight(true);
}

static void OnMenuKey(uint8 key, uint8 type)
{
	switch (type)
	{
	case HAL_KEY_STATE_PRESS:
		s_keyMenuStatus = HAL_KEY_STATE_PRESS;

		break;
		
	case HAL_KEY_STATE_LONG:
		s_keyMenuStatus = HAL_KEY_STATE_LONG;

		break;

	case HAL_KEY_STATE_RELEASE:
		if (s_keyMenuStatus == HAL_KEY_STATE_PRESS)
		{
			TRACE("key menu pressed\r\n");
	
			SwitchToMenu(s_prevMenuId);
		}

		s_keyMenuStatus = HAL_KEY_STATE_RELEASE;
		
		break;
	}
}

BEGIN_MENU_HANDLER(MENU_ID_LOCK)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

