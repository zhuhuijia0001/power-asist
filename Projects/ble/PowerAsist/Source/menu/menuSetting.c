#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "GUI.h"

#include "menuMessage.h"

#include "PowerAsist.h"

typedef enum
{
	Setting_Item_Time = 0,
	Setting_Item_Sample,
	Setting_Item_Screen,
	Setting_Item_Cali,
	Setting_Item_Ble,
	Setting_Item_Ver,

	Setting_Item_Back,
	
	Setting_Item_Count,

	Setting_Item_None
} SettingItem;
static SettingItem s_curSelSettingItem = Setting_Item_Time;

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void (*const s_drawSettingNormalItemFun[])() = 
{
	[Setting_Item_Time] = DrawSettingTimeNormalItem,
	[Setting_Item_Sample] = DrawSettingSampleNormalItem,
	[Setting_Item_Screen] = DrawSettingScreenNormalItem,
	[Setting_Item_Cali] = DrawSettingCalibrationNormalItem,
	[Setting_Item_Ble] = DrawSettingBleNormalItem,
	[Setting_Item_Ver] = DrawSettingVersionNormalItem,

	[Setting_Item_Back] = DrawSettingBackNormal,
};

static void (*const s_drawSettingSelectedItemFun[])() = 
{
	[Setting_Item_Time] = DrawSettingTimeSelItem,
	[Setting_Item_Sample] = DrawSettingSampleSelItem,
	[Setting_Item_Screen] = DrawSettingScreenSelItem,
	[Setting_Item_Cali] = DrawSettingCalibrationSelItem,
	[Setting_Item_Ble] = DrawSettingBleSelItem,
	[Setting_Item_Ver] = DrawSettingVersionSelItem,

	[Setting_Item_Back] = DrawSettingBackSel
};

static const MENU_ID s_menuId[] = 
{
	[Setting_Item_Time] = MENU_ID_TIME,
	[Setting_Item_Sample] = MENU_ID_SAMPLE,
	[Setting_Item_Screen] = MENU_ID_SCREEN,
	[Setting_Item_Cali] = MENU_ID_CALI,
	[Setting_Item_Ble] = MENU_ID_BLE,
	[Setting_Item_Ver] = MENU_ID_VERSION,
};

static const unsigned char *str_warning = "WARNING";

static const unsigned char *s_str_cali_warning = 
"DO NOT CALIBRATEWITHOUT HIGH\nPRECISION\nINSTRUMENT";

#define MESSAGE_LEFT   0
#define MESSAGE_TOP    22

static void MessageCallbackWarning(uint8 result)
{
	if (result == MESSAGE_OK)
	{
		SwitchToMenu(s_menuId[s_curSelSettingItem]);
	}
}

static void OnMenuCreate(MENU_ID prevId)
{
	if (prevId < MENU_ID_TIME || prevId > MENU_ID_VERSION)
	{
		s_prevMenuId = prevId;

		s_curSelSettingItem = Setting_Item_Time;
	}
	
	DrawSettingMenu();

	s_drawSettingSelectedItemFun[s_curSelSettingItem]();
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
			s_drawSettingNormalItemFun[s_curSelSettingItem]();
			s_curSelSettingItem += Setting_Item_Count - 1;
			s_curSelSettingItem %= Setting_Item_Count;
			s_drawSettingSelectedItemFun[s_curSelSettingItem]();
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelSettingItem == Setting_Item_Back)
			{
				SwitchToMenu(s_prevMenuId);
			}
			else
			{
				if (s_curSelSettingItem == Setting_Item_Cali)
				{
					EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, s_str_cali_warning, str_warning, MSG_TYPE_OK | MSG_TYPE_WARNING, MessageCallbackWarning);
				}
				else
				{
					SwitchToMenu(s_menuId[s_curSelSettingItem]);
				}
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
			s_drawSettingNormalItemFun[s_curSelSettingItem]();
			s_curSelSettingItem++;
			s_curSelSettingItem %= Setting_Item_Count;
			s_drawSettingSelectedItemFun[s_curSelSettingItem]();
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_SETTING)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

