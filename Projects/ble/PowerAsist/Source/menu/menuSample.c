
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "GUI.h"

#include "Parameter.h"

#include "npi.h"

typedef enum
{
	Sample_Item_Adc = 0,
	Sample_Item_Duration,

	Sample_Item_OK,
	Sample_Item_Cancel,
	
	Sample_Item_Count,

	Sample_Item_None
} SampleItem;

static SampleItem s_curSelItem = Sample_Item_None;
static SampleItem s_curEditItem = Sample_Item_None;

static uint8 s_sampleRate = SAMPLE_MIN;

static uint8 s_sampleDuration = SAMPLE_DURATION_MIN;

static void DrawNormalADC()
{
	DrawSampleNormalADC(s_sampleRate);
}

static void DrawNormalDuration()
{
	DrawSampleNormalDuration(s_sampleDuration);
}

static void (*const s_drawSampleNormalItemFun[])() =
{
	[Sample_Item_Adc] = DrawNormalADC,
	[Sample_Item_Duration] = DrawNormalDuration,

	[Sample_Item_OK] = DrawSampleNormalOK,
	[Sample_Item_Cancel] = DrawSampleNormalCancel,
};

static void DrawSelADC()
{
	DrawSampleSelADC(s_sampleRate);
}

static void DrawSelDuration()
{
	DrawSampleSelDuration(s_sampleDuration);
}

static void (*const s_drawSampleSelItemFun[])() =
{
	[Sample_Item_Adc] = DrawSelADC,
	[Sample_Item_Duration] = DrawSelDuration,

	[Sample_Item_OK] = DrawSampleSelOK,
	[Sample_Item_Cancel] = DrawSampleSelCancel,
};

//enter edit
static void EnterEditADC()
{
	DrawSampleEditADC(s_sampleRate);
}

static void EnterEditDuration()
{
	DrawSampleEditDuration(s_sampleDuration);
}

static void (*const s_enterSampleEditItemFun[])() =
{
	[Sample_Item_Adc] = EnterEditADC,
	[Sample_Item_Duration] = EnterEditDuration,
};

//edit
static void EditADC(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_sampleRate = (s_sampleRate - SAMPLE_MIN + (SAMPLE_MAX - SAMPLE_MIN + 1) - 1) % (SAMPLE_MAX - SAMPLE_MIN + 1) + SAMPLE_MIN;

		DrawSampleEditADC(s_sampleRate);
	}
	else if (key == KEY_RIGHT)
	{
		s_sampleRate = (s_sampleRate - SAMPLE_MIN + 1) % (SAMPLE_MAX - SAMPLE_MIN + 1) + SAMPLE_MIN;

		DrawSampleEditADC(s_sampleRate);
	}
}

static void EditDuration(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_sampleDuration -= SAMPLE_DURATION_MIN;
		s_sampleDuration += (SAMPLE_DURATION_MAX - SAMPLE_DURATION_MIN + 1) - 1;
		s_sampleDuration %= (SAMPLE_DURATION_MAX - SAMPLE_DURATION_MIN + 1);
		s_sampleDuration += SAMPLE_DURATION_MIN;

		DrawSampleEditDuration(s_sampleDuration);
	}
	else if (key == KEY_RIGHT)
	{
		s_sampleDuration -= SAMPLE_DURATION_MIN;
		s_sampleDuration++;
		s_sampleDuration %= (SAMPLE_DURATION_MAX - SAMPLE_DURATION_MIN + 1);
		s_sampleDuration += SAMPLE_DURATION_MIN;

		DrawSampleEditDuration(s_sampleDuration);
	}
}
	
static void (*const s_editSampleItemFun[])(uint8 key) =
{
	[Sample_Item_Adc] = EditADC,
	[Sample_Item_Duration] = EditDuration,
};

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;
	
	s_sampleRate = g_sampleRate;
	s_sampleDuration = g_peakValleySampleDuration;
	
	DrawSampleMenu();

	s_curEditItem = Sample_Item_None;
	s_curSelItem = Sample_Item_Adc;
	s_drawSampleSelItemFun[s_curSelItem]();

	DrawNormalDuration();
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
			if (s_curEditItem == Sample_Item_None)
			{
				s_drawSampleNormalItemFun[s_curSelItem]();
				s_curSelItem += Sample_Item_Count - 1;
				s_curSelItem %= Sample_Item_Count;
				s_drawSampleSelItemFun[s_curSelItem]();
			}
			else
			{
				s_editSampleItemFun[s_curEditItem](key);
			}
			
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Sample_Item_None)
			{
				s_editSampleItemFun[s_curEditItem](key);
			}
		
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelItem == Sample_Item_OK)
			{
				//save
				if (s_sampleRate != g_sampleRate
					|| s_sampleDuration != g_peakValleySampleDuration)
				{
				 	g_sampleRate = s_sampleRate;
				 	g_peakValleySampleDuration = s_sampleDuration;
				 	
					SaveParameter();
				}
				
				SwitchToMenu(s_prevMenuId);
			}
			else if (s_curSelItem == Sample_Item_Cancel)
			{
				SwitchToMenu(s_prevMenuId);
			}
			else
			{
				if (s_curEditItem == Sample_Item_None)
				{
					s_enterSampleEditItemFun[s_curSelItem]();

					s_curEditItem = s_curSelItem;
				}
				else
				{
					s_drawSampleSelItemFun[s_curSelItem]();

					s_curEditItem = Sample_Item_None;
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
			if (s_curEditItem == Sample_Item_None)
			{
				s_drawSampleNormalItemFun[s_curSelItem]();
				s_curSelItem++;
				s_curSelItem %= Sample_Item_Count;
				s_drawSampleSelItemFun[s_curSelItem]();
			}
			else
			{
				s_editSampleItemFun[s_curEditItem](key);
			}
		
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curEditItem != Sample_Item_None)
			{
				s_editSampleItemFun[s_curEditItem](key);
			}
		
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_SAMPLE)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

