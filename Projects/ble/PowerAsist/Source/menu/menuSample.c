
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
	
	Sample_Item_OK,
	Sample_Item_Cancel,
	
	Sample_Item_Count,

	Sample_Item_None
} SampleItem;

static SampleItem s_curSelItem = Sample_Item_None;
static SampleItem s_curEditItem = Sample_Item_None;

static uint8 s_validSampleRates[] = 
{
	[0] = SAMPLE_RATE_1,
	[1] = SAMPLE_RATE_2,
	[2] = SAMPLE_RATE_5,
	[3] = SAMPLE_RATE_10,
	[4] = SAMPLE_RATE_20,
};
static uint8 s_validSampleRateIndex = 0;

//draw normal adc
static void DrawNormalADC()
{
	DrawSampleNormalADC(s_validSampleRates[s_validSampleRateIndex]);
}

static void (*const s_drawSampleNormalItemFun[])() =
{
	[Sample_Item_Adc] = DrawNormalADC,
	
	[Sample_Item_OK] = DrawSampleNormalOK,
	[Sample_Item_Cancel] = DrawSampleNormalCancel,
};

//draw sel adc
static void DrawSelADC()
{
	DrawSampleSelADC(s_validSampleRates[s_validSampleRateIndex]);
}

static void (*const s_drawSampleSelItemFun[])() =
{
	[Sample_Item_Adc] = DrawSelADC,

	[Sample_Item_OK] = DrawSampleSelOK,
	[Sample_Item_Cancel] = DrawSampleSelCancel,
};

//enter edit
static void EnterEditADC()
{
	DrawSampleEditADC(s_validSampleRates[s_validSampleRateIndex]);
}

static void (*const s_enterSampleEditItemFun[])() =
{
	[Sample_Item_Adc] = EnterEditADC,
};

//edit
static void EditADC(uint8 key)
{
	if (key == KEY_LEFT)
	{
		s_validSampleRateIndex += sizeof(s_validSampleRates) / sizeof(s_validSampleRates[0]) - 1;
		s_validSampleRateIndex %= sizeof(s_validSampleRates) / sizeof(s_validSampleRates[0]);
		
		DrawSampleEditADC(s_validSampleRates[s_validSampleRateIndex]);
	}
	else if (key == KEY_RIGHT)
	{
		s_validSampleRateIndex++;
		s_validSampleRateIndex %= sizeof(s_validSampleRates) / sizeof(s_validSampleRates[0]);
		
		DrawSampleEditADC(s_validSampleRates[s_validSampleRateIndex]);
	}
}
	
static void (*const s_editSampleItemFun[])(uint8 key) =
{
	[Sample_Item_Adc] = EditADC,
};

static MENU_ID s_prevMenuId = MENU_ID_NONE;

static uint8 FindSampleRateIndex(uint8 sample)
{
	for (uint8 i = 0; i < sizeof(s_validSampleRates) / sizeof(s_validSampleRates[0]); i++)
	{
		if (s_validSampleRates[i] == sample)
		{
			return i;
		}
	}

	return 0xff;
}

static void OnMenuCreate(MENU_ID prevId)
{
	s_prevMenuId = prevId;

	uint8 index = FindSampleRateIndex(g_sampleRate);
	if (index == 0xff)
	{
		index = 0;
	}
	s_validSampleRateIndex = index;

	DrawSampleMenu();

	s_curEditItem = Sample_Item_None;
	s_curSelItem = Sample_Item_Adc;
	s_drawSampleSelItemFun[s_curSelItem]();
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
				if (s_validSampleRates[s_validSampleRateIndex] != g_sampleRate)
				{
				 	g_sampleRate = s_validSampleRates[s_validSampleRateIndex];
				 	
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
