
#include "hal_types.h"

#include "menuOp.h"
#include "menu.h"

/* menu */
typedef struct 
{
	MENU_ID menuId;

	void (*pMsgHandler)(MSG_ID id, void *msg);
} MENU;

#define DEF_MENU(id)                     \
static const MENU s_menu##id## =           \
{                                        \
	.menuId = id,                        \
	.pMsgHandler = Menu##id##Handler,    \
}

#define MENU_ITEM(id)   [##id##] = &(s_menu##id##)

/* current menu */
static const MENU *s_curMenu = NULL;

/* define menus */
DEF_MENU(MENU_ID_WELCOME);

DEF_MENU(MENU_ID_MAIN);
DEF_MENU(MENU_ID_DETAIL);
DEF_MENU(MENU_ID_PEAK);
DEF_MENU(MENU_ID_SLEEP);

DEF_MENU(MENU_ID_SNIFFER);
DEF_MENU(MENU_ID_QC20);
DEF_MENU(MENU_ID_QC30);
DEF_MENU(MENU_ID_PD_LIST);
DEF_MENU(MENU_ID_PD);
DEF_MENU(MENU_ID_AUTO);

DEF_MENU(MENU_ID_SETTING);
DEF_MENU(MENU_ID_TIME);
DEF_MENU(MENU_ID_SAMPLE);
DEF_MENU(MENU_ID_SCREEN);
DEF_MENU(MENU_ID_CALI);
DEF_MENU(MENU_ID_BLE);
DEF_MENU(MENU_ID_VERSION);

DEF_MENU(MENU_ID_BLE_COMM);

DEF_MENU(MENU_ID_MESSAGE);

//menu array
static const MENU *const s_menuArr[MENU_ID_COUNT] = 
{
	MENU_ITEM(MENU_ID_WELCOME),

	MENU_ITEM(MENU_ID_MAIN),
	MENU_ITEM(MENU_ID_DETAIL),
	MENU_ITEM(MENU_ID_PEAK),
	MENU_ITEM(MENU_ID_SLEEP),

	MENU_ITEM(MENU_ID_SNIFFER),
	MENU_ITEM(MENU_ID_QC20),
	MENU_ITEM(MENU_ID_QC30),
	MENU_ITEM(MENU_ID_PD_LIST),
	MENU_ITEM(MENU_ID_PD),
	MENU_ITEM(MENU_ID_AUTO),
	
	MENU_ITEM(MENU_ID_SETTING),
	MENU_ITEM(MENU_ID_TIME),
	MENU_ITEM(MENU_ID_SAMPLE),
	MENU_ITEM(MENU_ID_SCREEN),
	MENU_ITEM(MENU_ID_CALI),
	MENU_ITEM(MENU_ID_BLE),
	MENU_ITEM(MENU_ID_VERSION),

	MENU_ITEM(MENU_ID_BLE_COMM),
	
	MENU_ITEM(MENU_ID_MESSAGE),
};

void SwitchToMenu(MENU_ID id)
{
	const MENU *oldMenu = s_curMenu;
		
	MENU_ID oldId = MENU_ID_NONE;
	
	s_curMenu = s_menuArr[id];
	
	if (oldMenu != NULL)
	{		
		oldMenu->pMsgHandler(MSG_ID_DESTROY, (void *)id);
		
		oldId = oldMenu->menuId;
	}
	
	s_curMenu->pMsgHandler(MSG_ID_CREATE, (void *)oldId);
}

MENU_ID GetCurrentMenu()
{
	if (s_curMenu != NULL)
	{
		return s_curMenu->menuId;
	}
	
	return MENU_ID_NONE;
}

void ProcessCurMenuKey(uint8 key, uint8 type)
{
	uint16 k = key | (type << 8);

	if (s_curMenu != NULL)
	{
		s_curMenu->pMsgHandler(MSG_ID_KEY, (void *)k);
	}
}

void ProcessMenuTimeout(uint16 timerId)
{
	if (s_curMenu != NULL)
	{
		s_curMenu->pMsgHandler(MSG_ID_TIMEOUT, (void *)timerId);
	}
}

void ProcessMenuBleStatusChanged(uint16 status)
{
	if (s_curMenu != NULL)
	{
		s_curMenu->pMsgHandler(MSG_ID_BLE_STATUS_CHANGED, (void *)status);
	}
}

void ProcessMenuBleDataReceived(uint8 *buf, uint8 len)
{
	if (s_curMenu != NULL)
	{
		BleData data = 
		{
			.buf = buf,
			.len = len,
		};
		
		s_curMenu->pMsgHandler(MSG_ID_BLE_DATA_RECEIVED, (void *)&data);
	}
}

void InitMenu()
{
	SwitchToMenu(MENU_ID_WELCOME);
}

