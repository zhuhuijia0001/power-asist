
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "Parameter.h"
#include "draw.h"

#include "GUI.h"

#include "npi.h"

typedef enum
{
	Ble_Item_Switch = 0,

	Ble_Item_OK,
	Ble_Item_Cancel,
	
	Ble_Item_Count,

	Ble_Item_None
} BleItem;

static BleItem s_curSelItem = Ble_Item_None;

static BleItem s_curEditItem = Ble_Item_None;

static bool s_on = false;

static void DrawNormalSwitch()
{
	DrawBleNormalSwitch(s_on);
}

static void (*const s_drawBleNormalItemFun[])() =
{
	[Ble_Item_Switch] = DrawNormalSwitch,

	[Ble_Item_OK] = DrawBleNormalOK,
	[Ble_Item_Cancel] = DrawBleNormalCancel,
};

static void DrawSelSwitch()
{
	DrawBleSelSwitch(s_on);
}

static void (*const s_drawBleSelItemFun[])() =
{
	[Ble_Item_Switch] = DrawSelSwitch,

	[Ble_Item_OK] = DrawBleSelOK,
	[Ble_Item_Cancel] = DrawBleSelCancel,
};

//enter edit
static void EnterEditSwitch()
{
	DrawBleEditSwitch(s_on);
}

static void (*const s_enterBleEditItemFun[])() =
{
	[Ble_Item_Switch] = EnterEditSwitch,
};

//edit
static void EditSwitch(uint8 key)
{
	if (key == KEY_LEFT || key == KEY_RIGHT)
	{
		s_on = !s_on;

		DrawBleEditSwitch(s_on);
	}
}
	
static void (*const s_editBleItemFun[])(uint8 key) =
{
	[Ble_Item_Switch] = EditSwitch,
};

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;
	
	s_on = g_bleOn;
	
	DrawBleMenu();

	DrawBleNormalName(g_bleName);
	
	s_curEditItem = Ble_Item_None;
	s_curSelItem = Ble_Item_Switch;
	s_drawBleSelItemFun[s_curSelItem]();
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
			if (s_curEditItem == Ble_Item_None)
			{
				s_drawBleNormalItemFun[s_curSelItem]();
				s_curSelItem += Ble_Item_Count - 1;
				s_curSelItem %= Ble_Item_Count;
				s_drawBleSelItemFun[s_curSelItem]();
			}
			else
			{
				s_editBleItemFun[s_curEditItem](key);
			}
		
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Ble_Item_None)
			{
				s_editBleItemFun[s_curEditItem](key);
			}
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelItem == Ble_Item_OK)
			{
				//save
				if (s_on != g_bleOn)
				{
					g_bleOn = s_on;
					
					SaveParameter();
				}
				
				SwitchToMenu(s_prevMenuId);
			}
			else if (s_curSelItem == Ble_Item_Cancel)
			{
				SwitchToMenu(s_prevMenuId);
			}
			else
			{
				if (s_curEditItem == Ble_Item_None)
				{
					s_enterBleEditItemFun[s_curSelItem]();

					s_curEditItem = s_curSelItem;
				}
				else
				{
					s_drawBleSelItemFun[s_curSelItem]();

					s_curEditItem = Ble_Item_None;
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
			if (s_curEditItem == Ble_Item_None)
			{
				s_drawBleNormalItemFun[s_curSelItem]();
				s_curSelItem++;
				s_curSelItem %= Ble_Item_Count;
				s_drawBleSelItemFun[s_curSelItem]();
			}
			else
			{
				s_editBleItemFun[s_curEditItem](key);
			}
		
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Ble_Item_None)
			{
				s_editBleItemFun[s_curEditItem](key);
			}
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_BLE)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

