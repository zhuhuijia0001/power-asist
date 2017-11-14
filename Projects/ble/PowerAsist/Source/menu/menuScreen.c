
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "Parameter.h"
#include "draw.h"
#include "GUI.h"

typedef enum
{
	Screen_Item_Lock = 0,
	Screen_Item_Angle,

	Screen_Item_OK,
	Screen_Item_Cancel,
	
	Screen_Item_Count,

	Screen_Item_None
} ScreenItem;

static ScreenItem s_curSelItem = Screen_Item_None;

static ScreenItem s_curEditItem = Screen_Item_None;

static uint8 s_lock = LOCK_MAX;

static uint8 s_orientation = ORIENTATION_0;

static const uint16 s_orientationMap[] = 
{
	[ORIENTATION_0] = 0,
	[ORIENTATION_90] = 90,
	[ORIENTATION_180] = 180,
	[ORIENTATION_270] = 270,
};

static void DrawNormalLock()
{
	DrawScreenNormalLock(s_lock);
}

static void DrawNormalAngle()
{
	DrawScreenNormalAngle(s_orientationMap[s_orientation]);
}

static void (*const s_drawScreenNormalItemFun[])() =
{
	[Screen_Item_Lock] = DrawNormalLock,
	[Screen_Item_Angle] = DrawNormalAngle,

	[Screen_Item_OK] = DrawScreenNormalOK,
	[Screen_Item_Cancel] = DrawScreenNormalCancel,
};

static void DrawSelLock()
{
	DrawScreenSelLock(s_lock);
}

static void DrawSelAngle()
{
	DrawScreenSelAngle(s_orientationMap[s_orientation]);
}

static void (*const s_drawScreenSelItemFun[])() =
{
	[Screen_Item_Lock] = DrawSelLock,
	[Screen_Item_Angle] = DrawSelAngle,

	[Screen_Item_OK] = DrawScreenSelOK,
	[Screen_Item_Cancel] = DrawScreenSelCancel,
};

//enter edit
static void EnterEditLock()
{
	DrawScreenEditLock(s_lock);
}

static void EnterEditAngle()
{
	DrawScreenEditAngle(s_orientationMap[s_orientation]);
}

static void (*const s_enterScreenEditItemFun[])() =
{
	[Screen_Item_Lock] = EnterEditLock,
	[Screen_Item_Angle] = EnterEditAngle,
};


//edit
static void EditLock(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_lock = (s_lock - LOCK_MIN + (LOCK_MAX - LOCK_MIN + 1) - 1) % (LOCK_MAX - LOCK_MIN + 1) + LOCK_MIN;

		DrawScreenEditLock(s_lock);
	}
	else if (key == KEY_RIGHT)
	{
		s_lock = (s_lock - LOCK_MIN + 1) % (LOCK_MAX - LOCK_MIN + 1) + LOCK_MIN;

		DrawScreenEditLock(s_lock);
	}
}

static void EditAngle(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_orientation += ORIENTATION_COUNT - 1;
		s_orientation %= ORIENTATION_COUNT;

		DrawScreenEditAngle(s_orientationMap[s_orientation]);
	}
	else if (key == KEY_RIGHT)
	{
		s_orientation++;
		s_orientation %= ORIENTATION_COUNT;

		DrawScreenEditAngle(s_orientationMap[s_orientation]);
	}
}
	
static void (*const s_editScreenItemFun[])(uint8 key) =
{
	[Screen_Item_Lock] = EditLock,
	[Screen_Item_Angle] = EditAngle,
};

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;

	s_lock = g_screenLockTime;
	s_orientation = g_screenAngle;
	
	DrawScreenMenu();

	s_curSelItem = Screen_Item_Lock;
	s_curEditItem = Screen_Item_None;
	
	s_drawScreenSelItemFun[s_curSelItem]();

	DrawNormalAngle();
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
			if (s_curEditItem == Screen_Item_None)
			{
				s_drawScreenNormalItemFun[s_curSelItem]();
				s_curSelItem += Screen_Item_Count - 1;
				s_curSelItem %= Screen_Item_Count;
				s_drawScreenSelItemFun[s_curSelItem]();
			}
			else
			{
				s_editScreenItemFun[s_curEditItem](key);
			}
		
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Screen_Item_None)
			{
				s_editScreenItemFun[s_curEditItem](key);
			}
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelItem == Screen_Item_OK)
			{
				//save
				if (s_lock != g_screenLockTime
					|| s_orientation != g_screenAngle)
				{
					g_screenLockTime = s_lock;
					g_screenAngle = s_orientation;
					
					SaveParameter();
				}
				
				SetLcdOrientation((Orientation)s_orientation);
				
				SwitchToMenu(s_prevMenuId);
			}
			else if (s_curSelItem == Screen_Item_Cancel)
			{
				SwitchToMenu(s_prevMenuId);
			}
			else
			{
				if (s_curEditItem == Screen_Item_None)
				{
					s_enterScreenEditItemFun[s_curSelItem]();

					s_curEditItem = s_curSelItem;
				}
				else
				{
					s_drawScreenSelItemFun[s_curSelItem]();

					s_curEditItem = Screen_Item_None;
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
			if (s_curEditItem == Screen_Item_None)
			{
				s_drawScreenNormalItemFun[s_curSelItem]();
				s_curSelItem++;
				s_curSelItem %= Screen_Item_Count;
				s_drawScreenSelItemFun[s_curSelItem]();
			}
			else
			{
				s_editScreenItemFun[s_curEditItem](key);
			}
		
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Screen_Item_None)
			{
				s_editScreenItemFun[s_curEditItem](key);
			}
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_SCREEN)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

