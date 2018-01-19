#include "bcomdef.h"
#include "hal_key.h"

#include "OSAL.h"
#include "menuOp.h"
#include "menu.h"

#include "draw.h"

#include "LCD.h"
#include "GUI.h"

#include "QuickCharge.h"
#include "Meter.h"

#include "UsbPd.h"

#include "AutoDetect.h"

#include "Parameter.h"
#include "Protocol.h"
#include "Packet.h"

#include "powerAsistGATTprofile.h"

#include "PowerAsist.h"

#include "npi.h"

//timer id
#define BLE_COM_MENU_TIMERID_MEASURE      (POWERASIST_FIRST_TIMERID + 0)

#define BLE_COM_MENU_TIMERID_PARSE_PACKET (POWERASIST_FIRST_TIMERID + 1)

#define BLE_COM_MENU_PARSE_PACKET_TIMEOUT  5000ul

//current ble status
static uint8 s_curBleStatus = BLE_STATUS_INIT;

//current data bitmap
static uint8 s_curDataBitmap = 0x00;

//current mode 
static uint8 s_curMode = MODE_NORMAL;

//current voltage of mode 
static uint8 s_curModeVoltage = 0;

static void OnMenuCreate(MENU_ID prevId)
{
	s_curBleStatus = BLE_STATUS_INIT;

	g_bleOn = BLE_ON;
	SaveParameter();
	
	DrawBleComMenu();

	EnableBleAdvertise(true);
}

static void OnMenuDestroy(MENU_ID nextId)
{
	ClearScreen(BLACK);
}

static void OnMenuKey(uint8 key, uint8 type)
{
	if (key == KEY_MENU)
	{
		switch (type)
		{
		case HAL_KEY_STATE_PRESS:

			if (s_curBleStatus == BLE_STATUS_ADVERTISING)
			{
				g_bleOn = BLE_OFF;
				SaveParameter();

				EnableBleAdvertise(false);
				
				SwitchToMenu(GetMainMenu(g_mainMenu));
			}
			
			break;
		}
	}
}

static void SendBleData(uint8 *buf, uint8 len)
{
	/*
#ifdef DEBUG_PRINT
	TRACE("send ble data:");
	for (i = 0; i < len; i++)
	{
		TRACE("0x%02X ", data[i]);
	}
	TRACE("\r\n");
#endif
	*/

	uint8 segCnt = (len + 20 - 1) / 20;

	uint8 left = segCnt * 20 - len;
	uint8 i;
	for (i = 0; i < left; i++)
	{
		buf[len + i] = 0x00;
	}
	
	for (i = 0; i < segCnt; i++)
	{
		PowerAsistProfile_Notify(buf + i * 20, 20);
	}
}

void SendBleTestData(uint8 mode, uint8 modeVoltage, uint8 dataBitmap, const DataItem *data)
{
	if (dataBitmap != 0)
	{
		uint8 buf[MAX_PACKET_LEN];

		uint8 len = BuildTestDataPacket(buf, sizeof(buf), mode, modeVoltage, dataBitmap, data);
		
		SendBleData(buf, len);
	}
}

static DataItem s_dataItem[7];

static void OnMenuTimeout(uint16 timerId)
{
	if (timerId == BLE_COM_MENU_TIMERID_MEASURE)
	{	
		uint8 integer;
		uint16 integer16;
		
		uint16 frac;

		uint8 index = 0;		
		if (s_curDataBitmap & DATA_VOLTAGE_MASK)
		{
			GetBusVoltage(&integer, &frac);

			//TRACE("bus voltage:%d.%04dV\r\n", integer, frac);

			s_dataItem[index].dec = integer;
			s_dataItem[index].frac = frac;
			
			index++;
		}

		if (s_curDataBitmap & DATA_CURRENT_MASK)
		{
			GetLoadCurrent(&integer, &frac);

			//TRACE("current:%d.%04dA\r\n", integer, frac);
			
			s_dataItem[index].dec = integer;
			s_dataItem[index].frac = frac;
			
			index++;
		}

		if (s_curDataBitmap & DATA_POWER_MASK)
		{
			GetLoadPower(&integer, &frac);

			//TRACE("power:%d.%04dW\r\n", integer, frac);
			
			s_dataItem[index].dec = integer;
			s_dataItem[index].frac = frac;
			
			index++;
		}

		if (s_curDataBitmap & DATA_WH_MASK)
		{ 
			GetLoadWh(&integer16, &frac);
			
			s_dataItem[index].dec = integer;
			s_dataItem[index].frac = frac;
			
			index++;
		}

		if (s_curDataBitmap & DATA_AH_MASK)
		{ 
			GetLoadAh(&integer16, &frac);

			s_dataItem[index].dec = integer;
			s_dataItem[index].frac = frac;
			
			index++;
		}

		if (s_curDataBitmap & DATA_DP_MASK)
		{ 
			GetDPVoltage(&integer, &frac);

			//TRACE("dp voltage:%d.%02dV\r\n", integer, frac);
			
			s_dataItem[index].dec = integer;
			s_dataItem[index].frac = frac;
			
			index++;
		}

		if (s_curDataBitmap & DATA_DM_MASK)
		{ 
			GetDMVoltage(&integer, &frac);

			//TRACE("dm voltage:%d.%02dV\r\n", integer, frac);
			
			s_dataItem[index].dec = integer;
			s_dataItem[index].frac = frac;
			
			index++;
		}

		SendBleTestData(s_curMode, s_curModeVoltage, s_curDataBitmap, s_dataItem);
	}
	else if (timerId == BLE_COM_MENU_TIMERID_PARSE_PACKET)
	{
		ResetParsePacket();
	}
}

static void OnMenuBleStatusChanged(uint16 status)
{
	switch (status)
	{
	case BLE_STATUS_ADVERTISING:
		TRACE("advertising..\r\n");
		
		break;

	case BLE_STATUS_CONNECTED:
		TRACE("connected..\r\n");
		
		break;

	case BLE_STATUS_DISCONNECTED:
		TRACE("disconnected..\r\n");

		StopPowerAsistTimer(BLE_COM_MENU_TIMERID_MEASURE);
		
		break;
	}

	s_curBleStatus = status;
}


static bool s_qcEnabled = false;

static void QC20Callback(bool supported)
{
	if (supported)
	{
		TRACE("qc2.0 supported\r\n");

		SetCurrentSnifferStatus(SNIFFER_QC_20);

		//default is 5V
		SetCurrentSnifferTargetVoltage(QC20_5V);
		s_curModeVoltage = QC20_5V;
		
		s_qcEnabled = true;
	}
	else
	{
		StopQC20Sniffer();
		TRACE("qc2.0 not supported\r\n");
	}
}

//process qc20 enable
static void ProcessQc20Enable(uint8 enable)
{
	if (enable == QUICK_CHARGE_ENABLE)
	{
		StartQC20Sniffer(QC20Callback);
	}
	else
	{
		StopQC20Sniffer();
		
		SetCurrentSnifferStatus(SNIFFER_NONE);
		//default is 5V
		SetCurrentSnifferTargetVoltage(QC20_5V);
		s_curModeVoltage = 0;
		
		s_qcEnabled = false;
	}

	uint8 buf[MAX_PACKET_LEN];

	uint8 len = BuildEnableQc20RetPacket(buf, sizeof(buf), enable, RESULT_OK);

	SendBleData(buf, len);
}

static void QC30Callback(bool supported)
{
	if (supported)
	{
		TRACE("qc2.0 supported\r\n");

		SetQC30Mode();

		SetCurrentSnifferStatus(SNIFFER_QC_30);
		SetCurrentSnifferTargetVoltage(0);

		s_curModeVoltage = 0;
		
		s_qcEnabled = true;
	}
	else
	{
		StopQC20Sniffer();
		TRACE("qc2.0 not supported\r\n");
	}
}

//process qc30 enable
static void ProcessQc30Enable(uint8 enable)
{
	if (enable == QUICK_CHARGE_ENABLE)
	{
		StartQC20Sniffer(QC30Callback);
	}
	else
	{
		StopQC20Sniffer();

		SetCurrentSnifferStatus(SNIFFER_NONE);
		//default is 5V
		SetCurrentSnifferTargetVoltage(QC20_5V);
		s_curModeVoltage = 0;
		
		s_qcEnabled = false;
	}

	uint8 buf[MAX_PACKET_LEN];

	uint8 len = BuildEnableQc30RetPacket(buf, sizeof(buf), enable, RESULT_OK);

	SendBleData(buf, len);
}

static PDItem s_pdItem[MAX_CAPABILITIES];
static uint8 s_pdCnt = 0;

static void PdCapsCallback(uint8 res, const CapabilityList *list)
{
	uint8 buf[MAX_PACKET_LEN];

	uint8 len;

	s_pdCnt = 0;
	
	if (res == GET_CAPS_OK)
	{
		TRACE("get caps OK,%d items\r\n", list->cnt);

		uint8 i;
		for (i = 0; i < list->cnt; i++)
		{
			s_pdItem[i].voltage = list->caps[i].voltage / 1000;
			s_pdItem[i].current = list->caps[i].maxMa;
		}
		s_pdCnt = list->cnt;

		SetCurrentSnifferStatus(SNIFFER_PD);
		SetCurrentSnifferTargetVoltage(s_pdItem[0].voltage);

		s_curModeVoltage = s_pdItem[0].voltage;
		
		len = BuildEnablePdRetPacket(buf, sizeof(buf), PD_ENABLE, RESULT_OK, s_pdItem, s_pdCnt);
	}
	else if (res == GET_CAPS_FAILED)
	{
		TRACE("get caps failed\r\n");

		len = BuildEnablePdRetPacket(buf, sizeof(buf), PD_ENABLE, RESULT_FAILED, NULL, 0);
	}

	SendBleData(buf, len);
}

//process pd enable
static void ProcessPdEnable(uint8 enable)
{
	if (enable == PD_ENABLE)
	{
		EnterUsbPd(PdCapsCallback);
	}
	else
	{
		ExitUsbPd();

		SetCurrentSnifferStatus(SNIFFER_NONE);
		SetCurrentSnifferTargetVoltage(QC20_5V);

		s_curModeVoltage = 0;
		
		uint8 buf[MAX_PACKET_LEN];

		uint8 len = BuildEnablePdRetPacket(buf, sizeof(buf), PD_DISABLE, RESULT_OK, NULL, 0);
		SendBleData(buf, len);
	}
}

static void PdRequestCallback(uint8 requestedVoltage, uint8 result)
{
	uint8 ret;
	bool needRet = false;
	
	switch (result)
	{
	case REQUEST_ACCEPT:	
		ret = RESULT_OK;
		needRet = true;

		SetCurrentSnifferTargetVoltage(requestedVoltage);
		s_curModeVoltage = requestedVoltage;
		
		break;
		
	case REQUEST_REJECT:
	case REQUEST_FAILED:
		ret = RESULT_FAILED;
		needRet = false;
		
		break;
	}

	if (needRet)
	{
		uint8 buf[MAX_PACKET_LEN];

		uint8 len = BuildTriggerRetPacket(buf, sizeof(buf), MODE_PD, requestedVoltage, ret);
		SendBleData(buf, len);
	}
}

//process trigger 
static void ProcessTrigger(uint8 mode, uint8 modeVoltage)
{
	uint8 buf[MAX_PACKET_LEN];
	
	bool result = false;
	
	switch (mode)
	{
	case MODE_QC20:
		switch (modeVoltage)
		{
		case QC20_5V:
			if (s_qcEnabled)
			{
				SetQC20Sniffer5V();

				SetCurrentSnifferTargetVoltage(QC20_5V);
				s_curModeVoltage = QC20_5V;

				result = true;
			}
			
			break;

		case QC20_9V:
			if (s_qcEnabled)
			{
				SetQC20Sniffer9V();

				SetCurrentSnifferTargetVoltage(QC20_9V);
				s_curModeVoltage = QC20_9V;
				
				result = true;
			}
			
			break;

		case QC20_12V:
			if (s_qcEnabled)
			{
				SetQC20Sniffer12V();

				SetCurrentSnifferTargetVoltage(QC20_12V);
				s_curModeVoltage = QC20_12V;
				
				result = true;
			}
			
			break;

		case QC20_20V:
			if (s_qcEnabled)
			{
				SetQC20Sniffer20V();

				SetCurrentSnifferTargetVoltage(QC20_20V);
				s_curModeVoltage = QC20_20V;
				
				result = true;
			}
			
			break;
		}
		
		break;

	case MODE_QC30:
		switch (modeVoltage)
		{
		case QC30_INCREASE:
			if (s_qcEnabled)
			{
				IncreaseQC30Voltage();

				result = true;
			}
			
			break;

		case QC30_DECREASE:
			if (s_qcEnabled)
			{
				DecreaseQC30Voltage();

				result = true;
			}
			
			break;
		}
		
		break;

	case MODE_PD:
		{
			uint8 i;
			for (i = 0; i < s_pdCnt; i++)
			{
				if (s_pdItem[i].voltage == modeVoltage)
				{

					break;
				}
			}

			if (i < s_pdCnt)
			{
				//can request				
				PdRequest(i + 1, PdRequestCallback);
			}
			else
			{
				//not found
				uint8 len = BuildTriggerRetPacket(buf, sizeof(buf), mode, modeVoltage, RESULT_FAILED);

				SendBleData(buf, len);
			}
		}
		
		break;

	default:

		break;
	}

	if (mode == MODE_QC20 || mode == MODE_QC30)
	{
		uint8 len = BuildTriggerRetPacket(buf, sizeof(buf), mode, modeVoltage, result ? RESULT_OK : RESULT_FAILED);
		SendBleData(buf, len);
	}
}

static void AutoItemCallback(bool support, uint8 mode, uint8 voltage)
{
	uint8 buf[MAX_PACKET_LEN];
	uint8 len;
	uint8 result;
	if (support)
	{
		result = MODE_SUPPORT;
	}
	else
	{
		result = MODE_UNSUPPORT;
	}

	len = BuildAutoDetectRetPacket(buf, sizeof(buf), mode, voltage, result);
	SendBleData(buf, len);
}

static void AutoDcpCallback(bool support)
{
	AutoItemCallback(support, MODE_DCP, 0);
}

static void AutoQc20Callback(bool support)
{
	AutoItemCallback(support, MODE_QC20, 0);
}

static void AutoQc209VCallback(bool support)
{
	AutoItemCallback(support, MODE_QC20, QC20_9V);
}

static void AutoQc2012VCallback(bool support)
{
	AutoItemCallback(support, MODE_QC20, QC20_12V);
}

static void AutoQc2020VCallback(bool support)
{
	AutoItemCallback(support, MODE_QC20, QC20_20V);
}

static void AutoQc30Callback(bool support)
{
	AutoItemCallback(support, MODE_QC30, 0);
}

static void AutoApple21ACallback(bool support)
{
	AutoItemCallback(support, MODE_APPLE_2_1A, 0);
}

static void AutoPdCallback(bool support)
{
	AutoItemCallback(support, MODE_PD, 0);
}

static void AutoCompleteCallback(bool support)
{
	AutoItemCallback(support, MODE_NORMAL, 0);

	//Start accumulate
	StartAccumulateWhAndAh();
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

//process auto detect
static void ProcessAutoDetect()
{
	//stop accumulate
	StopAccumulateWhAndAh();
	
	StartAutoDetect(AutoDetectCallback);
}

//process time setting
static void ProcessSetTime(const TimeStruct *time)
{
	uint8 result;

	TRACE("%d-%d-%d %d:%d:%d\r\n", time->year, time->month, time->day,
			time->hour, time->minute, time->second);
			
	if (SetRTCTime(time))
	{
		TRACE("set time OK\r\n");

		result = RESULT_OK;
	}
	else
	{
		TRACE("set time failed\r\n");

		result = RESULT_FAILED;
	}

	uint8 buf[MAX_PACKET_LEN];
	uint8 len = BuildSetTimeRetPacket(buf, sizeof(buf), result);
	SendBleData(buf, len);
}

//process query param
static void ProcessQueryParam()
{
	uint8 buf[MAX_PACKET_LEN];

	uint8 ver[FIRMWARE_VER_LEN];
	osal_memset(ver, 0, sizeof(ver));
	osal_memcpy(ver, FIRMWARE_VER, osal_strlen(FIRMWARE_VER));
	uint8 len = BuildQueryParamRetPacket(buf, sizeof(buf), g_sampleRate, SAMPLE_DURATION_MAX, ver);
	SendBleData(buf, len);				
}

static bool IsValidSampleRate(uint8 sample)
{
	if (sample == SAMPLE_RATE_20
		|| sample == SAMPLE_RATE_10
		|| sample == SAMPLE_RATE_5
		|| sample == SAMPLE_RATE_2
		|| sample == SAMPLE_RATE_1)
	{
		return true;
	}

	return false;
}

//process set sample
static void ProcessSetSample(uint8 sample)
{
	if (!IsValidSampleRate(sample))
	{
		sample = DEFAULT_SAMPLE_RATE;
	}
	
	if (sample != g_sampleRate)
	{
		g_sampleRate = sample;
		SaveParameter();

		uint32 sampleInterval = 1000ul / g_sampleRate;
		StartPowerAsistTimer(BLE_COM_MENU_TIMERID_MEASURE, sampleInterval, true);
	}

	uint8 buf[MAX_PACKET_LEN];
	uint8 len = BuildSetSampleRetPacket(buf, sizeof(buf), RESULT_OK);
	SendBleData(buf, len);
}

//process set ble name
static void ProcessSetBleName(const uint8 name[MAX_BLE_NAME_LEN])
{
	UpdateBleName(name);

	osal_memcpy(g_bleName, name, MAX_BLE_NAME_LEN);
	SaveParameter();
	
	uint8 buf[MAX_PACKET_LEN];
	uint8 len = BuildSetBleNameRetPacket(buf, sizeof(buf), RESULT_OK);
	SendBleData(buf, len);
}

//process query charge mode
static void ProcessQueryChargeMode()
{
	uint8 mode = MODE_NORMAL;
	SnifferStatus status = GetCurrentSnifferStatus();
	switch (status)
	{
	case SNIFFER_QC_20:
		mode = MODE_QC20;
		break;

	case SNIFFER_QC_30:
		mode = MODE_QC30;
		break;

	case SNIFFER_PD:
		mode = MODE_PD;
		break;
	}
	uint8 voltage = GetCurrentSnifferTargetVoltage();

	uint8 buf[MAX_PACKET_LEN];
	uint8 len = BuildQueryChargeModeRetPacket(buf, sizeof(buf), mode, voltage, RESULT_OK);
	SendBleData(buf, len);
}

static void ProcessBleCom(const uint8 *buf, uint8 len)
{
	//for echo test
#if 0
	PowerAsistProfile_Notify(buf, len);
#else

	uint8 parsedLen;

	uint8 res = ParsePacket(buf, len, &parsedLen);
	if (res == PACKET_OK)
	{
		StopPowerAsistTimer(BLE_COM_MENU_TIMERID_PARSE_PACKET);
		
		uint8 type = GetPacketType();
		uint8 dataLen = GetPacketDataLen();
		const uint8 *data = GetPacketData();

		//TRACE("type:%d,dataLen:%d,data[0]:0x%02X\r\n", type, dataLen, data[0]);
		
		switch (type)
		{
		case TYPE_ENABLE_TRANSFER:
			{
				uint8 dataBitmap;

				if (ParseEnableTransferPacket(data, dataLen, &dataBitmap))
				{
					TRACE("parse enable transfer ok,datamap:0x%02X\r\n", dataBitmap);
					
					if (dataBitmap)
					{
						uint32 sampleInterval = 1000ul / g_sampleRate;
						StartPowerAsistTimer(BLE_COM_MENU_TIMERID_MEASURE, sampleInterval, true);
					}
					else
					{
						StopPowerAsistTimer(BLE_COM_MENU_TIMERID_MEASURE);
					}

					s_curDataBitmap = dataBitmap;
				}
			}

			break;

		case TYPE_ENABLE_QC20:
			{
				uint8 enable;
				if (ParseEnableQc20Packet(data, dataLen, &enable))
				{
					TRACE("Parse qc2.0 enable:%d\r\n", enable);
					
					ProcessQc20Enable(enable);
				}
				else
				{
					TRACE("parse qc2.0 failed\r\n");
				}
			}
			
			break;

		case TYPE_ENABLE_QC30:
			{
				uint8 enable;
				if (ParseEnableQc30Packet(data, dataLen, &enable))
				{
					TRACE("Parse qc3.0 enable:%d\r\n", enable);
					
					ProcessQc30Enable(enable);
				}
				else
				{
					TRACE("parse qc3.0 failed\r\n");
				}
			}
			
			break;

		case TYPE_ENABLE_PD:
			{
				uint8 enable;
				if (ParseEnablePdPacket(data, dataLen, &enable))
				{
					TRACE("Parse pd enable:%d\r\n", enable);

					ProcessPdEnable(enable);
				}
				else
				{
					TRACE("Parse pd failed:%d\r\n", enable);
				}
			}
			
			break;

		case TYPE_TRIGGER_QUICK_CHARGE:
			{
				uint8 mode;
				uint8 modeVoltage;
				if (ParseTriggerPacket(data, dataLen, &mode, &modeVoltage))
				{
					TRACE("Parse trigger,mode:%d,voltage:%d\r\n", mode, modeVoltage);
					
					ProcessTrigger(mode, modeVoltage);
				}
				else
				{
					TRACE("Parse trigger failed\r\n");
				}
			}
			
			break;

		case TYPE_AUTO_DETECT:
			{
				TRACE("Auto detect\r\n");
				
				ProcessAutoDetect();
			}
			
			break;

		case TYPE_SET_TIME:
			{
				TimeStruct time;	
				if (ParseSetTimePacket(data, dataLen, &time))
				{
					TRACE("Parse set time OK\r\n");
					
					ProcessSetTime(&time);
				} 
				else
				{
					TRACE("Parse set time failed\r\n");
				}
			}
			
			break;

		case TYPE_QUERY_PARAM:
			{
				ProcessQueryParam();
			}
			
			break;

		case TYPE_SET_SAMPLE:
			{
				uint8 sample;
				if (ParseSetSamplePacket(data, dataLen, &sample))
				{
					TRACE("Parse set sample OK\r\n");
					
					ProcessSetSample(sample);
				}
				else
				{
					TRACE("Parse set sample failed\r\n");
				}
			}
			
			break;
			
		case TYPE_SET_BLE_NAME:
			{
				uint8 name[MAX_BLE_NAME_LEN];
				if (ParseSetBleNamePacket(data, dataLen, name))
				{
					TRACE("Parse set name OK\r\n");
					
					ProcessSetBleName(name);
				}
				else
				{
					TRACE("Parse set name failed\r\n");
				}
			}
			
			break;

		case TYPE_QUERY_CHARGE_MODE:
			{
				ProcessQueryChargeMode();
			}
			
			break;

		default:
			break;
		}
	}
	else if (res == PACKET_INVALID)
	{
		TRACE("invalid packet\r\n");
		StopPowerAsistTimer(BLE_COM_MENU_TIMERID_PARSE_PACKET);
	}
	else
	{
		//incomplete
		TRACE("incomplete packet\r\n");

		StartPowerAsistTimer(BLE_COM_MENU_TIMERID_PARSE_PACKET, BLE_COM_MENU_PARSE_PACKET_TIMEOUT, false);
	}
#endif
}

static void OnMenuBleDataReceived(uint8 *buf, uint8 len)
{
/*
	#ifdef DEBUG_PRINT
	uint8 i;
	TRACE("recv ble data:\r\n");
	for (i = 0; i < len; i++)
	{
		TRACE("0x%02X ", buf[i]);
	}
	TRACE("\r\n");
	
	#endif
*/		
	ProcessBleCom(buf, len);
}

BEGIN_MENU_HANDLER(MENU_ID_BLE_COMM)

ON_MENU_CREATE()
ON_MENU_DESTROY()
ON_MENU_KEY()
ON_MENU_TIMEOUT()
ON_MENU_BLE_STATUS_CHANGED()
ON_MENU_BLE_DATA_RECEIVED()

END_MENU_HANDLER()

