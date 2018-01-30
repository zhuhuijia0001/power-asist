
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "GUI.h"

#include "npi.h"

typedef enum
{
	Cali_Item_Voltage = 0,
	Cali_Item_Current,

	Cali_Item_Back,
	
	Cali_Item_Count,

	Cali_Item_None
} CaliItem;
static CaliItem s_curSelCaliItem = Cali_Item_Voltage;

static void (*const s_drawCaliNormalItemFun[])() = 
{
	[Cali_Item_Voltage] = DrawCaliNormalVoltage,
	[Cali_Item_Current] = DrawCaliNormalCurrent,

	[Cali_Item_Back] = DrawCaliNormalBack,
};

static void (*const s_drawCaliSelectedItemFun[])() = 
{
	[Cali_Item_Voltage] = DrawCaliSelVoltage,
	[Cali_Item_Current] = DrawCaliSelCurrent,

	[Cali_Item_Back] = DrawCaliSelBack,
};

static const MENU_ID s_menuId[] = 
{
	[Cali_Item_Voltage] = MENU_ID_VOLTAGE_CALI,
	[Cali_Item_Current] = MENU_ID_CURRENT_CALI,
};

static void OnMenuCreate(MENU_ID prevId)
{
	if (prevId < MENU_ID_VOLTAGE_CALI 
		|| prevId > MENU_ID_CURRENT_CALI)
	{
		s_curSelCaliItem = Cali_Item_Voltage;
	}
	
	DrawCaliMenu();

	s_drawCaliSelectedItemFun[s_curSelCaliItem]();
}

static void OnMenuDestroy(MENU_ID nextId)
{
	ClearScreen(BLACK);
}

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_LEFT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			s_drawCaliNormalItemFun[s_curSelCaliItem]();
			s_curSelCaliItem += Cali_Item_Count - 1;
			s_curSelCaliItem %= Cali_Item_Count;
			s_drawCaliSelectedItemFun[s_curSelCaliItem]();
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelCaliItem == Cali_Item_Back)
			{
				SwitchToMenu(MENU_ID_SETTING);
			}
			else
			{
				SwitchToMenu(s_menuId[s_curSelCaliItem]);
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
			s_drawCaliNormalItemFun[s_curSelCaliItem]();
			s_curSelCaliItem++;
			s_curSelCaliItem %= Cali_Item_Count;
			s_drawCaliSelectedItemFun[s_curSelCaliItem]();
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_CALI)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

