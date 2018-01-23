
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
static const uint8 *s_caption;

static uint8 s_msg_type;

static void (*s_resultCallback)(uint8 result) = NULL;

void EnterMessageMenu(uint16 x, uint16 y, const uint8 *msg, const uint8 *caption, uint8 msgType, void (*resultCallback)(uint8 result))
{
	s_left = x;
	s_top = y;

	s_msg = msg;
	s_caption = caption;
	
	s_msg_type = msgType;
	
	s_resultCallback = resultCallback;
	
	SwitchToMenu(MENU_ID_MESSAGE);
}

typedef enum
{
	msg_Item_ok = 0,
	msg_Item_yes = msg_Item_ok,
	msg_Item_cancel, 
	msg_Item_no = msg_Item_cancel,
	
	msg_Item_Count,

	msg_Item_None
} MsgItem;

static MsgItem s_curSelItem = msg_Item_None;

static void (*const s_drawMessageNormalOkCancelFun[])() =
{
	[msg_Item_ok] = DrawMessageNormalOK,
	[msg_Item_cancel] = DrawMessageNormalCancel,
};

static void (*const s_drawMessageSelOkCancelFun[])() =
{
	[msg_Item_ok] = DrawMessageSelOK,
	[msg_Item_cancel] = DrawMessageSelCancel,
};


static void (*const s_drawMessageNormalYesNoFun[])() =
{
	[msg_Item_yes] = DrawMessageNormalYes,
	[msg_Item_no] = DrawMessageNormalNo,
};

static void (*const s_drawMessageSelYesNoFun[])() =
{
	[msg_Item_yes] = DrawMessageSelYes,
	[msg_Item_no] = DrawMessageSelNo,
};

static void OnMenuCreate(MENU_ID prevId)
{	
	if (s_caption != NULL)
	{
		uint8 captionType = s_msg_type & 0xf0;
		uint16 fc = WHITE;
		if (captionType & MSG_TYPE_INFORMATION)
		{
			fc = CYAN;
		}
		else if (captionType & MSG_TYPE_WARNING)
		{
			fc = RED;
		}

		DrawMessageCaption(s_caption, fc);
	}

	if (s_msg != NULL)
	{
		DrawMessageMsg(s_left, s_top, s_msg);
	}
	
	uint8 msgType = s_msg_type & 0x0f;
	if (msgType & MSG_TYPE_OK)
	{
		s_curSelItem = msg_Item_None;
		DrawMessageSelOK();
	}
	else if (msgType & MSG_TYPE_OK_CANCEL)
	{
		s_curSelItem = msg_Item_ok;
		DrawMessageSelOK();
		DrawMessageNormalCancel();
	}
	else if (msgType & MSG_TYPE_YES_NO)
	{
		s_curSelItem = msg_Item_yes;
		DrawMessageSelYes();
		DrawMessageNormalNo();
	}
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

			if (s_msg_type & MSG_TYPE_OK_CANCEL)
			{
				s_drawMessageNormalOkCancelFun[s_curSelItem]();
				s_curSelItem += msg_Item_Count - 1;
				s_curSelItem %= msg_Item_Count;
				s_drawMessageSelOkCancelFun[s_curSelItem]();
			}
			else if (s_msg_type == MSG_TYPE_YES_NO)
			{
				s_drawMessageNormalYesNoFun[s_curSelItem]();
				s_curSelItem += msg_Item_Count - 1;
				s_curSelItem %= msg_Item_Count;
				s_drawMessageSelYesNoFun[s_curSelItem]();
			}
			
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

			if (s_msg_type & MSG_TYPE_OK)
			{
				result = MESSAGE_OK;

				if (s_resultCallback != NULL)
				{
					s_resultCallback(result);
				}
			}
			else if (s_msg_type & MSG_TYPE_OK_CANCEL)
			{
				if (s_curSelItem == msg_Item_ok)
				{
					result = MESSAGE_OK;
				}
				else
				{
					result = MESSAGE_CANCEL;
				}

				if (s_resultCallback != NULL)
				{
					s_resultCallback(result);
				}
			}
			else if (s_msg_type == MSG_TYPE_YES_NO)
			{
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

			if (s_msg_type & MSG_TYPE_OK_CANCEL)
			{
				s_drawMessageNormalOkCancelFun[s_curSelItem]();
				s_curSelItem++;
				s_curSelItem %= msg_Item_Count;
				s_drawMessageSelOkCancelFun[s_curSelItem]();
			}
			else if (s_msg_type == MSG_TYPE_YES_NO)
			{
				s_drawMessageNormalYesNoFun[s_curSelItem]();
				s_curSelItem++;
				s_curSelItem %= msg_Item_Count;
				s_drawMessageSelYesNoFun[s_curSelItem]();
			}
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_MESSAGE)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

