
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "GUI.h"

#include "RTC.h"

typedef enum
{
	Time_Item_Year = 0,
	Time_Item_Month,
	Time_Item_Day,
	Time_Item_Hour,
	Time_Item_Minute,
	Time_Item_Second,

	Time_Item_OK,
	Time_Item_Cancel,
	
	Time_Item_Count,

	Time_Item_None
} TimeItem;
static TimeItem s_curSelItem = Time_Item_None;

static TimeItem s_curEditItem = Time_Item_None;

static TimeStruct s_time;

static bool	IsLeapYear(uint16 yr)
{
 	return (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)));
}

static void DrawNormalYear()
{
	DrawTimeNormalYear(s_time.year);
}

static void DrawNormalMonth()
{
	DrawTimeNormalMonth(s_time.month);
}

static void DrawNormalDay()
{
	DrawTimeNormalDay(s_time.day);
}

static void DrawNormalHour()
{
	DrawTimeNormalHour(s_time.hour);
}

static void DrawNormalMinute()
{
	DrawTimeNormalMinute(s_time.minute);
}

static void DrawNormalSecond()
{
	DrawTimeNormalSecond(s_time.second);
}

static void (*const s_drawTimeNormalItemFun[])() =
{
	[Time_Item_Year] = DrawNormalYear,
	[Time_Item_Month] = DrawNormalMonth,
	[Time_Item_Day] = DrawNormalDay,
	[Time_Item_Hour] = DrawNormalHour,
	[Time_Item_Minute] = DrawNormalMinute,
	[Time_Item_Second] = DrawNormalSecond,

	[Time_Item_OK] = DrawTimeNormalOK,
	[Time_Item_Cancel] = DrawTimeNormalCancel,
};

static void DrawSelYear()
{
	DrawTimeSelYear(s_time.year);
}

static void DrawSelMonth()
{
	DrawTimeSelMonth(s_time.month);
}

static void DrawSelDay()
{
	DrawTimeSelDay(s_time.day);
}

static void DrawSelHour()
{
	DrawTimeSelHour(s_time.hour);
}

static void DrawSelMinute()
{
	DrawTimeSelMinute(s_time.minute);
}

static void DrawSelSecond()
{
	DrawTimeSelSecond(s_time.second);
}

static void (*const s_drawTimeSelectedItemFun[])() =
{
	[Time_Item_Year] = DrawSelYear,
	[Time_Item_Month] = DrawSelMonth,
	[Time_Item_Day] = DrawSelDay,
	[Time_Item_Hour] = DrawSelHour,
	[Time_Item_Minute] = DrawSelMinute,
	[Time_Item_Second] = DrawSelSecond,

	[Time_Item_OK] = DrawTimeSelOK,
	[Time_Item_Cancel] = DrawTimeSelCancel,
};

//enter edit
static void EnterEditYear()
{
	DrawTimeEditYear(s_time.year);
}

static void EnterEditMonth()
{
	DrawTimeEditMonth(s_time.month);
}

static void EnterEditDay()
{
	DrawTimeEditDay(s_time.day);
}

static void EnterEditHour()
{
	DrawTimeEditHour(s_time.hour);
}

static void EnterEditMinute()
{
	DrawTimeEditMinute(s_time.minute);
}

static void EnterEditSecond()
{
	DrawTimeEditSecond(s_time.second);
}

static void (*const s_enterTimeEditItemFun[])() =
{
	[Time_Item_Year] = EnterEditYear,
	[Time_Item_Month] = EnterEditMonth,
	[Time_Item_Day] = EnterEditDay,
	[Time_Item_Hour] = EnterEditHour,
	[Time_Item_Minute] = EnterEditMinute,
	[Time_Item_Second] = EnterEditSecond,
};

//edit
static void EditYear(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_time.year = (s_time.year - 2000 + 100 - 1) % 100 + 2000;

		DrawTimeEditYear(s_time.year);
	}
	else if (key == KEY_RIGHT)
	{
		s_time.year = (s_time.year - 2000 + 1) % 100 + 2000;

		DrawTimeEditYear(s_time.year);
	}
}

static uint8 AdjustDay(uint16 year, uint8 month, uint8 day)
{
	if (month > 7)
	{
		month--;
	}

	if (month == 2)
	{
		uint8 upLimit = 28;
		if (IsLeapYear(year))
		{
			upLimit = 29;
		}

		if (day > upLimit)
		{
			day = upLimit;
		}
	}
	else if ((month & 0x01) == 0x00)
	{
		if (day > 30)
		{
			day = 30;
		}
	}

	return day;
}

static uint8 GetDayCountOfMonth(uint16 year, uint8 month)
{
	uint8 days = 31;
	
	if (month == 2) // feb
	{
	  	days = 28;
	  	if (IsLeapYear(year))
	  	{
	  		days++;
	  	}
	}
	else
	{
		if (month > 7) // aug-dec
	  	{
			month--;
	  	}
	
	  	if ((month & 0x01) == 0)
		{
			days = 30;
	  	}
	}
	
	return days;
}

static void EditMonth(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_time.month = (s_time.month - 1 + 12 - 1) % 12 + 1;

		DrawTimeEditMonth(s_time.month);

		uint8 day = AdjustDay(s_time.year, s_time.month, s_time.day);
		if (day != s_time.day)
		{
			DrawTimeNormalDay(day);

			s_time.day = day;
		}
	}
	else if (key == KEY_RIGHT)
	{
		s_time.month = (s_time.month - 1 + 1) % 12 + 1;

		DrawTimeEditMonth(s_time.month);

		uint8 day = AdjustDay(s_time.year, s_time.month, s_time.day);
		if (day != s_time.day)
		{
			DrawTimeNormalDay(day);

			s_time.day = day;
		}
	}
}

static void EditDay(uint8 key)
{
	if (key == KEY_LEFT)
	{
		uint8 days = GetDayCountOfMonth(s_time.year, s_time.month);		
		s_time.day = (s_time.day - 1 + days - 1) % days + 1;

		DrawTimeEditDay(s_time.day);
	}
	else if (key == KEY_RIGHT)
	{	
		uint8 days = GetDayCountOfMonth(s_time.year, s_time.month);
		s_time.day = (s_time.day - 1 + 1) % days + 1;

		DrawTimeEditDay(s_time.day);
	}
}

static void EditHour(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_time.hour = (s_time.hour + 24 - 1) % 24;

		DrawTimeEditHour(s_time.hour);
	}
	else if (key == KEY_RIGHT)
	{
		s_time.hour = (s_time.hour + 1) % 24;

		DrawTimeEditHour(s_time.hour);
	}
}

static void EditMinute(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_time.minute = (s_time.minute + 60 - 1) % 60;

		DrawTimeEditMinute(s_time.minute);
	}
	else if (key == KEY_RIGHT)
	{
		s_time.minute = (s_time.minute + 1) % 60;

		DrawTimeEditMinute(s_time.minute);
	}
}

static void EditSecond(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_time.second = (s_time.second + 60 - 1) % 60;

		DrawTimeEditSecond(s_time.second);
	}
	else if (key == KEY_RIGHT)
	{
		s_time.second = (s_time.second + 1) % 60;

		DrawTimeEditSecond(s_time.second);
	}
}

static void (*const s_editTimeItemFun[])(uint8 key) =
{
	[Time_Item_Year] = EditYear,
	[Time_Item_Month] = EditMonth,
	[Time_Item_Day] = EditDay,
	[Time_Item_Hour] = EditHour,
	[Time_Item_Minute] = EditMinute,
	[Time_Item_Second] = EditSecond,
};

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;
	
	GetRTCTime(&s_time);
	
	DrawTimeMenu(&s_time);

	s_curSelItem = Time_Item_Year;
	s_curEditItem = Time_Item_None;
	
	s_drawTimeSelectedItemFun[s_curSelItem]();
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
			if (s_curEditItem == Time_Item_None)
			{
				s_drawTimeNormalItemFun[s_curSelItem]();
				s_curSelItem += Time_Item_Count - 1;
				s_curSelItem %= Time_Item_Count;
				s_drawTimeSelectedItemFun[s_curSelItem]();
			}
			else
			{
				s_editTimeItemFun[s_curEditItem](key);
			}
		
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Time_Item_None)
			{
				s_editTimeItemFun[s_curEditItem](key);
			}
			
			break;
		}		
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelItem == Time_Item_OK)
			{
				SetRTCTime(&s_time);

				SwitchToMenu(s_prevMenuId);
			}
			else if (s_curSelItem == Time_Item_Cancel)
			{
				SwitchToMenu(s_prevMenuId);
			}
			else
			{
				if (s_curEditItem == Time_Item_None)
				{
					s_enterTimeEditItemFun[s_curSelItem]();

					s_curEditItem = s_curSelItem;
				}
				else
				{
					s_drawTimeSelectedItemFun[s_curSelItem]();

					s_curEditItem = Time_Item_None;
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
			if (s_curEditItem == Time_Item_None)
			{
				s_drawTimeNormalItemFun[s_curSelItem]();
				s_curSelItem++;
				s_curSelItem %= Time_Item_Count;
				s_drawTimeSelectedItemFun[s_curSelItem]();
			}
			else
			{
				s_editTimeItemFun[s_curEditItem](key);
			}
		
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Time_Item_None)
			{
				s_editTimeItemFun[s_curEditItem](key);
			}
			
			break;
		}	
	}
}

BEGIN_MENU_HANDLER(MENU_ID_TIME)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

