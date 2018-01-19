
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

static uint8 s_validLockTime[] = 
{
	[0] = LOCK_1_MIN,
	[1] = LOCK_2_MIN,
	[2] = LOCK_3_MIN,
	[3] = LOCK_4_MIN,
	[4] = LOCK_5_MIN,
	[5] = LOCK_NEVER,
};
static uint8 s_validLockTimeIndex = 0;

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
	DrawScreenNormalLock(s_validLockTime[s_validLockTimeIndex]);
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
	DrawScreenSelLock(s_validLockTime[s_validLockTimeIndex]);
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
	DrawScreenEditLock(s_validLockTime[s_validLockTimeIndex]);
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
		s_validLockTimeIndex += sizeof(s_validLockTime) / sizeof(s_validLockTime[0]) - 1;
		s_validLockTimeIndex %= sizeof(s_validLockTime) / sizeof(s_validLockTime[0]);
		
		DrawScreenEditLock(s_validLockTime[s_validLockTimeIndex]);
	}
	else if (key == KEY_RIGHT)
	{
		s_validLockTimeIndex++;
		s_validLockTimeIndex %= sizeof(s_validLockTime) / sizeof(s_validLockTime[0]);

		DrawScreenEditLock(s_validLockTime[s_validLockTimeIndex]);
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

static uint8 FindLockTimeIndex(uint8 minute)
{
	for (uint8 i = 0; i < sizeof(s_validLockTime) / sizeof(s_validLockTime[0]); i++)
	{
		if (s_validLockTime[i] == minute)
		{
			return i;
		}
	}

	return 0xff;
}

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;

	uint8 index = FindLockTimeIndex(g_screenLockTime);
	if (index == 0xff)
	{
		index = 0;
	}
	s_validLockTimeIndex = index;
	
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
				if (s_validLockTime[s_validLockTimeIndex] != g_screenLockTime
					|| s_orientation != g_screenAngle)
				{
					g_screenLockTime = s_validLockTime[s_validLockTimeIndex];
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

