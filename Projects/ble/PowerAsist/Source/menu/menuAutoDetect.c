
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "LCD.h"
#include "GUI.h"

#include "Meter.h"
#include "AutoDetect.h"

#include "Parameter.h"

#include "Global.h"

#include "powerAsistGATTprofile.h"
#include "PowerAsist.h"

#include "npi.h"

//whether auto detect completes
static bool s_detectComplete = false;

static void AutoDcpCallback(bool support)
{
	if (support)
	{
		DrawAutoDcpSupport();
	}
	else
	{
		DrawAutoDcpUnsupport();
	}
}

static void AutoQc20Callback(bool support)
{
	if (support)
	{
		DrawAutoQc20Support();
	}
	else
	{			
		DrawAutoQc20Unsupport();
		//DrawAutoQc209VUnsupport();
		//DrawAutoQc2012VUnsupport();
		//DrawAutoQc2020VUnsupport();

		//DrawAutoQc30Unsupport();
	}
}

static void AutoQc209VCallback(bool support)
{
	if (support)
	{
		DrawAutoQc209VSupport();
	}
	else
	{
		DrawAutoQc209VUnsupport();
	}
}

static void AutoQc2012VCallback(bool support)
{
	if (support)
	{
		DrawAutoQc2012VSupport();
	}
	else
	{
		DrawAutoQc2012VUnsupport();
	}
}

static void AutoQc2020VCallback(bool support)
{
	if (support)
	{
		DrawAutoQc2020VSupport();
	}
	else
	{
		DrawAutoQc2020VUnsupport();
	}
}

static void AutoQc30Callback(bool support)
{
	if (support)
	{
		DrawAutoQc30Support();
	}
	else
	{
		DrawAutoQc30Unsupport();
	}
}

static void AutoApple21ACallback(bool support)
{
	if (support)
	{
		DrawAutoApple21ASupport();
	}
	else
	{
		DrawAutoApple21AUnsupport();
	}
}

static void AutoPdCallback(bool support)
{
	if (support)
	{
		DrawAutoPdSupport();
	}
	else
	{
		DrawAutoPdUnsupport();
	}
}

static void AutoCompleteCallback(bool support)
{
	DrawAutoSelBack();
		
	s_detectComplete = true;
}

static void (* const s_callbackArr[])(bool support) = 
{
	[detect_type_dcp] = AutoDcpCallback,
	[detect_type_qc20] = AutoQc20Callback,
	[detect_type_qc20_9v] = AutoQc209VCallback,
	[detect_type_qc20_12v] = AutoQc2012VCallback,
	[detect_type_qc20_20v] = AutoQc2020VCallback,
	[detect_type_qc30] = AutoQc30Callback,
	[detect_type_apple_21a] = AutoApple21ACallback,
	[detect_type_pd] = AutoPdCallback,
	[detect_type_none] = AutoCompleteCallback,
};

static void AutoDetectCallback(DetectType type, bool support)
{
	if (type < sizeof(s_callbackArr) / sizeof(s_callbackArr[0]))
	{
		s_callbackArr[type](support);
	}
}

static wh_ah_status s_savedStatus;

static void OnMenuCreate(MENU_ID prevId)
{
	g_autoDetect = AUTO_DETECT_ON;
	SaveParameter();
				
	//stop accumulate
	s_savedStatus = GetWhAndAhStatus();
	if (s_savedStatus == WH_AH_STATUS_STARTED)
	{
		StopAccumulateWhAndAh();
	}
	
	DrawAutoDetectMenu();

	s_detectComplete = false;
	StartAutoDetect(AutoDetectCallback);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	ClearScreen(BLACK);

	//Start accumulate
	if (s_savedStatus == WH_AH_STATUS_STARTED)
	{
		StartAccumulateWhAndAh();
	}
}

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_detectComplete)
			{
				g_autoDetect = AUTO_DETECT_OFF;
				SaveParameter();
		
				SwitchToMenu(GetMainMenu(g_mainMenu));
			}
			
			break;
		}
	}
}

BEGIN_MENU_HANDLER(MENU_ID_AUTO)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()

END_MENU_HANDLER()

