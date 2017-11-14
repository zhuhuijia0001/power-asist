
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "GUI.h"

#include "menuMessage.h"
#include "npi.h"

static uint16 s_left;
static uint16 s_top;

static const uint8 *s_msg;

static void (*s_resultCallback)(uint8 result) = NULL;

void EnterMessageMenu(uint16 x, uint16 y, const uint8 *msg, void (*resultCallback)(uint8 result))
{
	s_left = x;
	s_top = y;

	s_msg = msg;

	s_resultCallback = resultCallback;
	
	SwitchToMenu(MENU_ID_MESSAGE);
}

typedef enum
{
	msg_Item_yes = 0,
	msg_Item_no,
	
	msg_Item_Count,

	msg_Item_None
} MsgItem;

static MsgItem s_curSelItem = msg_Item_None;

static void (*const s_drawMessageNormalItemFun[])() =
{
	[msg_Item_yes] = DrawMessageNormalYes,
	[msg_Item_no] = DrawMessageNormalNo,
};

static void (*const s_drawMessageSelItemFun[])() =
{
	[msg_Item_yes] = DrawMessageSelYes,
	[msg_Item_no] = DrawMessageSelNo,
};

static void OnMenuCreate(MENU_ID prevId)
{	
	DrawMessageMenu(s_left, s_top, s_msg);

	s_curSelItem = msg_Item_yes;
	s_drawMessageSelItemFun[s_curSelItem]();
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
			TRACE("key left pressed\r\n");

			s_drawMessageNormalItemFun[s_curSelItem]();
			s_curSelItem += msg_Item_Count - 1;
			s_curSelItem %= msg_Item_Count;
			s_drawMessageSelItemFun[s_curSelItem]();
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		uint8 result;
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			TRACE("key menu pressed\r\n");

			if (s_curSelItem == msg_Item_yes)
			{
				result = MESSAGE_YES;
			}
			else
			{
				result = MESSAGE_NO;
			}

			if (s_resultCallback != NULL)
			{
				s_resultCallback(result);
			}
			
			break;
		}
	}
	else if (key == KEY_RIGHT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			TRACE("key right pressed\r\n");

			s_drawMessageNormalItemFun[s_curSelItem]();
			s_curSelItem++;
			s_curSelItem %= msg_Item_Count;
			s_drawMessageSelItemFun[s_curSelItem]();
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_MESSAGE)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

