
#include "bcomdef.h"
#include "hal_key.h"

#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "LCD.h"
#include "GUI.h"

#include "Meter.h"

#include "QuickCharge.h"

#include "menuMessage.h"
#include "Global.h"

#include "Parameter.h"

#include "powerAsistGATTprofile.h"
#include "PowerAsist.h"

#include "npi.h"

#define QC30_MENU_TIMERID          (POWERASIST_FIRST_TIMERID + 0)

typedef enum
{
	qc30_Item_decrease = 0,
	qc30_Item_increase,

	qc30_Item_back,
	
	qc30_Item_Count,

	qc30_Item_None
} Qc30Item;

static Qc30Item s_curSelItem = qc30_Item_None;

static void (*const s_drawQC30NormalItemFun[])() =
{
	[qc30_Item_decrease] = DrawQC30NormalDecrease,
	[qc30_Item_increase] = DrawQC30NormalIncrease,

	[qc30_Item_back] = DrawQC30NormalBack,
};

static void (*const s_drawQC30SelItemFun[])() =
{
	[qc30_Item_decrease] = DrawQC30SelDecrease,
	[qc30_Item_increase] = DrawQC30SelIncrease,

	[qc30_Item_back] = DrawQC30SelBack,
};

static void (*const s_drawQC30PressItemFun[])() =
{
	[qc30_Item_decrease] = DrawQC30PressDecrease,
	[qc30_Item_increase] = DrawQC30PressIncrease,
};

static void (*const s_snifferItemFun[])() = 
{
	[qc30_Item_decrease] = DecreaseQC30Voltage,
	[qc30_Item_increase] = IncreaseQC30Voltage,
};

static bool s_qcChecked = false;
static void QC20Callback(bool supported)
{
	if (supported)
	{
		SetQC30Mode();

		SetCurrentSnifferStatus(SNIFFER_QC_30);
		SetCurrentSnifferTargetVoltage(0);
		
		TRACE("qc2.0 supported\r\n");
	}
	else
	{
		StopQC20Sniffer();
		TRACE("qc2.0 not supported\r\n");
	}

	s_qcChecked = true;
}

static void RefreshQC30MenuContent()
{	
	uint8 integer;
	uint16 frac;
	
	if (GetBusVoltage(&integer, &frac))
	{
		DrawQC30Voltage(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		DrawQC30Current(integer, frac);
	}
}

static void OnMenuCreate(MENU_ID prevId)
{
	//stop accumulate
	StopAccumulateWhAndAh();
	
	DrawQC30Menu();

	s_curSelItem = qc30_Item_decrease;
	s_drawQC30SelItemFun[s_curSelItem]();

	RefreshQC30MenuContent();

	uint32 sampleInterval = 1000ul / g_sampleRate;
	StartPowerAsistTimer(QC30_MENU_TIMERID, sampleInterval, true);

	s_qcChecked = false;
	StartQC20Sniffer(QC20Callback);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	StopPowerAsistTimer(QC30_MENU_TIMERID);
	
	ClearScreen(BLACK);

	//Start accumulate
	StartAccumulateWhAndAh();
}

static void MessageCallback(uint8 result)
{
	if (result == MESSAGE_YES)
	{
		StopQC20Sniffer();

		SetCurrentSnifferStatus(SNIFFER_NONE);
	}
	
	SwitchToMenu(GetMainMenu(g_mainMenu));
}

#define MESSAGE_LEFT   0
#define MESSAGE_TOP    30

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_LEFT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			s_drawQC30NormalItemFun[s_curSelItem]();
			s_curSelItem += qc30_Item_Count - 1;
			s_curSelItem %= qc30_Item_Count;
			s_drawQC30SelItemFun[s_curSelItem]();
			
			break;
		}
	}	
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelItem == qc30_Item_back)
			{
				if (s_qcChecked)
				{
					if (GetCurrentSnifferStatus() != SNIFFER_NONE)
					{
						EnterMessageMenu(MESSAGE_LEFT, MESSAGE_TOP, "RELEASE SNIFFING?", MessageCallback);
					}
					else
					{
						SwitchToMenu(GetMainMenu(g_mainMenu));
					}
				}
			}
			else
			{
				s_drawQC30PressItemFun[s_curSelItem]();

				if (GetCurrentSnifferStatus() == SNIFFER_QC_30)
				{
					s_snifferItemFun[s_curSelItem]();
				}
			}
			
			break;

		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			if (s_curSelItem != qc30_Item_back)
			{
				s_drawQC30PressItemFun[s_curSelItem]();

				if (GetCurrentSnifferStatus() == SNIFFER_QC_30)
				{
					s_snifferItemFun[s_curSelItem]();
				}
			}
			
			break;
			
		case HAL_KEY_STATE_RELEASE:
			if (s_curSelItem != qc30_Item_back)
			{
				s_drawQC30SelItemFun[s_curSelItem]();
			}
			
			break;
		}
	}
	else if (key == KEY_RIGHT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			s_drawQC30NormalItemFun[s_curSelItem]();
			s_curSelItem++;
			s_curSelItem %= qc30_Item_Count;
			s_drawQC30SelItemFun[s_curSelItem]();
			
			break;
		}
	}
}

static void OnMenuTimeout(uint16 timerId)
{
	RefreshQC30MenuContent();
}

BEGIN_MENU_HANDLER(MENU_ID_QC30)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

