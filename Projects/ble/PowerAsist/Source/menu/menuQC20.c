
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

#include "Protocol.h"
#include "Parameter.h"

#include "powerAsistGATTprofile.h"
#include "PowerAsist.h"

#include "npi.h"

#define QC20_MENU_TIMERID          (POWERASIST_FIRST_TIMERID + 0)

typedef enum
{
	qc20_Item_5v = 0,
	qc20_Item_9v,
	qc20_Item_12v,
	qc20_Item_20v,

	qc20_Item_back,
	
	qc20_Item_Count,

	qc20_Item_None
} Qc20Item;

static Qc20Item s_curSelItem = qc20_Item_None;

static void (*const s_drawQC20NormalItemFun[])() =
{
	[qc20_Item_5v] = DrawQC20ItemNormal5V,
	[qc20_Item_9v] = DrawQC20ItemNormal9V,
	[qc20_Item_12v] = DrawQC20ItemNormal12V,
	[qc20_Item_20v] = DrawQC20ItemNormal20V,

	[qc20_Item_back] = DrawQC20NormalBack,
};

static void (*const s_drawQC20SelItemFun[])() =
{
	[qc20_Item_5v] = DrawQC20ItemSel5V,
	[qc20_Item_9v] = DrawQC20ItemSel9V,
	[qc20_Item_12v] = DrawQC20ItemSel12V,
	[qc20_Item_20v] = DrawQC20ItemSel20V,

	[qc20_Item_back] = DrawQC20SelBack,
};

static void (*const s_drawQC20PressItemFun[])() =
{
	[qc20_Item_5v] = DrawQC20ItemPress5V,
	[qc20_Item_9v] = DrawQC20ItemPress9V,
	[qc20_Item_12v] = DrawQC20ItemPress12V,
	[qc20_Item_20v] = DrawQC20ItemPress20V,
};

static void (*const s_snifferItemFun[])() = 
{
	[qc20_Item_5v] = SetQC20Sniffer5V,
	[qc20_Item_9v] = SetQC20Sniffer9V,
	[qc20_Item_12v] = SetQC20Sniffer12V,
	[qc20_Item_20v] = SetQC20Sniffer20V,
};

static void RefreshQC20MenuContent()
{	
	uint8 integer;
	uint16 frac;
	
	if (GetBusVoltage(&integer, &frac))
	{
		DrawQC20Voltage(integer, frac);
	}
	
	if (GetLoadCurrent(&integer, &frac))
	{
		DrawQC20Current(integer, frac);
	}
}

static bool s_qcChecked = false;

static void QC20Callback(bool supported)
{
	if (supported)
	{
		TRACE("qc2.0 supported\r\n");

		SetCurrentSnifferStatus(SNIFFER_QC_20);

		//default is 5V
		SetCurrentSnifferTargetVoltage(QC20_5V);
	}
	else
	{
		StopQC20Sniffer();
		TRACE("qc2.0 not supported\r\n");
	}

	s_qcChecked = true;
}

static void OnMenuCreate(MENU_ID prevId)
{
	//stop accumulate
	StopAccumulateWhAndAh();
	
	DrawQC20Menu();
	s_curSelItem = qc20_Item_5v;
	s_drawQC20SelItemFun[s_curSelItem]();
	
	RefreshQC20MenuContent();

	uint32 sampleInterval = 1000ul / g_sampleRate;
	StartPowerAsistTimer(QC20_MENU_TIMERID, sampleInterval, true);

	s_qcChecked = false;
	StartQC20Sniffer(QC20Callback);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	StopPowerAsistTimer(QC20_MENU_TIMERID);
	
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
		SetCurrentSnifferTargetVoltage(0);
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
		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			s_drawQC20NormalItemFun[s_curSelItem]();
			s_curSelItem += qc20_Item_Count - 1;
			s_curSelItem %= qc20_Item_Count;
			s_drawQC20SelItemFun[s_curSelItem]();
			
			break;
		}
	}
	else if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
			if (s_curSelItem == qc20_Item_back)
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
				s_drawQC20PressItemFun[s_curSelItem]();

				if (GetCurrentSnifferStatus() == SNIFFER_QC_20)
				{
					s_snifferItemFun[s_curSelItem]();

					uint8 voltage = 0;
					switch (s_curSelItem)
					{
					case qc20_Item_5v:
						voltage = QC20_5V;
						break;
						
					case qc20_Item_9v:
						voltage = QC20_9V;
						break;
						
					case qc20_Item_12v:
						voltage = QC20_12V;
						break;
						
					case qc20_Item_20v:
						voltage = QC20_20V;
						break;
					}

					SetCurrentSnifferTargetVoltage(voltage);
				}
			}
			
			break;

		case HAL_KEY_STATE_RELEASE:
			if (s_curSelItem != qc20_Item_back)
			{
				s_drawQC20SelItemFun[s_curSelItem]();
			}
			
			break;
		}
	}
	else if (key == KEY_RIGHT)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:
		case HAL_KEY_STATE_LONG:
		case HAL_KEY_STATE_CONTINUE:
			s_drawQC20NormalItemFun[s_curSelItem]();
			s_curSelItem++;
			s_curSelItem %= qc20_Item_Count;
			s_drawQC20SelItemFun[s_curSelItem]();
			
			break;
		}
	}
}

static void OnMenuTimeout(uint16 timerId)
{
	RefreshQC20MenuContent();
}

BEGIN_MENU_HANDLER(MENU_ID_QC20)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()

END_MENU_HANDLER()

