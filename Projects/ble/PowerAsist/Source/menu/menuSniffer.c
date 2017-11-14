
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

typedef enum
{
	Sniffer_Item_QC20 = 0,
	Sniffer_Item_QC30,
	Sniffer_Item_PD,
	Sniffer_Item_Auto,
	
	Sniffer_Item_Back,
	
	Sniffer_Item_Count,

	Sniffer_Item_None
} SnifferItem;
static SnifferItem s_curSelItem = Sniffer_Item_QC20;

static void (*const s_drawSnifferNormalItemFun[])() = 
{
	[Sniffer_Item_QC20] = DrawSnifferNormalQC20,
	[Sniffer_Item_QC30] = DrawSnifferNormalQC30,
	[Sniffer_Item_PD] = DrawSnifferNormalPD,
	[Sniffer_Item_Auto] = DrawSnifferNormalAutoDetect,
	
	[Sniffer_Item_Back] = DrawSnifferNormalBack,
};

static void (*const s_drawSnifferSelectedItemFun[])() = 
{
	[Sniffer_Item_QC20] = DrawSnifferSelQC20,
	[Sniffer_Item_QC30] = DrawSnifferSelQC30,
	[Sniffer_Item_PD] = DrawSnifferSelPD,
	[Sniffer_Item_Auto] = DrawSnifferSelAutoDetect,

	[Sniffer_Item_Back] = DrawSnifferSelBack
};

static const MENU_ID s_menuId[] = 
{
	[Sniffer_Item_QC20] = MENU_ID_QC20,
	[Sniffer_Item_QC30] = MENU_ID_QC30,
	[Sniffer_Item_PD] = MENU_ID_PD_LIST,
	[Sniffer_Item_Auto] = MENU_ID_AUTO,
};

static void OnMenuCreate(MENU_ID prevId)
{	
	DrawSnifferMenu();

	s_curSelItem = Sniffer_Item_QC20;
	s_drawSnifferSelectedItemFun[s_curSelItem]();
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
			s_drawSnifferNormalItemFun[s_curSelItem]();
			s_curSelItem += Sniffer_Item_Count - 1;
			s_curSelItem %= Sniffer_Item_Count;
			s_drawSnifferSelectedItemFun[s_curSelItem]();
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelItem == Sniffer_Item_Back)
			{
				SwitchToMenu(GetMainMenu(g_mainMenu));
			}
			else
			{
				SwitchToMenu(s_menuId[s_curSelItem]);
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
			s_drawSnifferNormalItemFun[s_curSelItem]();
			s_curSelItem++;
			s_curSelItem %= Sniffer_Item_Count;
			s_drawSnifferSelectedItemFun[s_curSelItem]();
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_SNIFFER)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

