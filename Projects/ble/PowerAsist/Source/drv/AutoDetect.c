#include "hal_types.h"

#include "OSAL.h"

#include "hal_drivers.h"

#include "QuickCharge.h"
#include "Detect.h"

#include "Meter.h"

#include "UsbPd.h"

#include "AutoDetect.h"

#include "Protocol.h"

#include "npi.h"

static void (*s_callback)(DetectType type, bool support) = NULL;

#define AUTO_DETECT_INTERVAL          1000ul
#define AUTO_DETECT_RELEASE_INTERVAL  500ul

//unit 100uV
#define QC30_STEP     2000

//auto detect, DCP, QC2.0 9V 12V 20V, QC3.0, Apple 2.1A, PD 
typedef enum
{
	status_detect_none = 0,
	
	status_detect_dcp,
	status_detect_qc20,
	status_detect_qc20_9V,
	status_detect_qc20_12V,
	status_detect_qc20_20V,
	status_detect_qc20_restore,
	status_detect_qc30_increase_before,
	status_detect_qc30_increase_after,
	status_detect_release_qc30,
	status_detect_apple_21A,
	status_detect_pd,
} DetectStatus;

//current detect status
static DetectStatus s_status = status_detect_none;

//usb pd callback
static void DetectUsbPdCallback(uint8 res, const CapabilityList *list)
{
	if (res == GET_CAPS_OK)
	{
		TRACE("usb pd supported\r\n");
		
		if (s_callback != NULL)
		{
			s_callback(detect_type_pd, true);
		}
	}
	else
	{
		TRACE("usb pd not supported\r\n");
		
		if (s_callback != NULL)
		{
			s_callback(detect_type_pd, false);
		}
	}

	ExitUsbPd();

	if (s_callback != NULL)
	{
		s_callback(detect_type_none, true);
	}
	
	s_status = status_detect_none;
}

//apple 2.1a callback
static void DetectApple21ACallback(DType type)
{
	if (type == d_type_apple_21A)
	{
		TRACE("apple 2.1A supported\r\n");
		
		if (s_callback != NULL)
		{
			s_callback(detect_type_apple_21a, true);
		}
	}
	else
	{
		TRACE("apple 2.1A not supported\r\n");

		//other
		if (s_callback != NULL)
		{
			s_callback(detect_type_apple_21a, false);
		}
	}

	s_status = status_detect_pd;
	EnterUsbPd(DetectUsbPdCallback);
}

//qc2.0 callback
static void QC20Callback(bool supported)
{
	if (supported)
	{
		TRACE("qc2.0 supported\r\n");
		if (s_callback != NULL)
		{
			s_callback(detect_type_qc20, true);
		}
		
		s_status = status_detect_qc20_9V;
		SetQC20Sniffer9V();
		osal_start_timerEx(Hal_TaskID, AUTO_DETECT_CHECK_EVT, AUTO_DETECT_INTERVAL);
	}
	else
	{
		StopQC20Sniffer();
		TRACE("qc2.0 not supported\r\n");

		if (s_callback != NULL)
		{
			s_callback(detect_type_qc20, false);

			s_callback(detect_type_qc20_9v, false);

			s_callback(detect_type_qc20_12v, false);

			s_callback(detect_type_qc20_20v, false);

			s_callback(detect_type_qc30, false);
		}

		s_status = status_detect_apple_21A;
		StartDetectDType(DetectApple21ACallback);
	}
}

//dcp callback
static void DetectDcpCallback(DType type)
{
	if (type == d_type_dcp)
	{
		TRACE("dcp supported\r\n");
		
		if (s_callback != NULL)
		{
			s_callback(detect_type_dcp, true);
		}
	}
	else
	{
		TRACE("dcp not supported\r\n");

		//other
		if (s_callback != NULL)
		{
			s_callback(detect_type_dcp, false);
		}
	}

	s_status = status_detect_qc20;
	StartQC20Sniffer(QC20Callback);
}


void StartAutoDetect(void (*callback)(DetectType type, bool support))
{
	s_callback = callback;

	s_status = status_detect_dcp;
	StartDetectDType(DetectDcpCallback);
}

//qc2.0 target voltage delta(V)
static const uint8 QC20_TARGET_VOLTAGE_DELTA = 1u;

//qc3.0 target voltage delta(0.1mV)
static const uint16 QC30_TARGET_VOLTAGE_DELTA = 500ul;

void AutoDetectTimeout()
{
	static uint32 qc30VoltageSaved = 0;
	
	uint8 integer;
	uint16 frac;
	
	GetBusVoltage(&integer, &frac);
	
	switch (s_status)
	{
	case status_detect_qc20_9V:
		TRACE("voltage:%d.%d\r\n", integer, frac);
		if (integer >= QC20_9V - QC20_TARGET_VOLTAGE_DELTA && integer <= QC20_9V + QC20_TARGET_VOLTAGE_DELTA)
		{
			//qc2.0 9V OK
			TRACE("qc2.0 9V supported\r\n");
			
			if (s_callback != NULL)
			{
				s_callback(detect_type_qc20_9v, true);
			}
		}
		else
		{
			TRACE("qc2.0 9V not supported\r\n");
			
			if (s_callback != NULL)
			{
				s_callback(detect_type_qc20_9v, false);
			}
		}
		
		s_status = status_detect_qc20_12V;
		SetQC20Sniffer12V();
		osal_start_timerEx(Hal_TaskID, AUTO_DETECT_CHECK_EVT, AUTO_DETECT_INTERVAL);
		
		break;

	case status_detect_qc20_12V:
		TRACE("voltage:%d.%d\r\n", integer, frac);
		if (integer >= QC20_12V - QC20_TARGET_VOLTAGE_DELTA && integer <= QC20_12V + QC20_TARGET_VOLTAGE_DELTA)
		{
			//qc2.0 12V OK
			TRACE("qc2.0 12V supported\r\n");
			
			if (s_callback != NULL)
			{
				s_callback(detect_type_qc20_12v, true);
			}
		}
		else
		{
			TRACE("qc2.0 12V not supported\r\n");
			
			if (s_callback != NULL)
			{
				s_callback(detect_type_qc20_12v, false);
			}
		}

		s_status = status_detect_qc20_20V;
		SetQC20Sniffer20V();
		osal_start_timerEx(Hal_TaskID, AUTO_DETECT_CHECK_EVT, AUTO_DETECT_INTERVAL);
		
		break;

	case status_detect_qc20_20V:
		TRACE("voltage:%d.%d\r\n", integer, frac);
		if (integer >= QC20_20V - QC20_TARGET_VOLTAGE_DELTA && integer <= QC20_20V + QC20_TARGET_VOLTAGE_DELTA)
		{
			//qc2.0 20V OK
			TRACE("qc2.0 20V supported\r\n");
			
			if (s_callback != NULL)
			{
				s_callback(detect_type_qc20_20v, true);
			}
		}
		else
		{
			TRACE("qc2.0 20V not supported\r\n");
			
			if (s_callback != NULL)
			{
				s_callback(detect_type_qc20_20v, false);
			}
		}

		s_status = status_detect_qc20_restore;
		SetQC20Sniffer5V();
		osal_start_timerEx(Hal_TaskID, AUTO_DETECT_CHECK_EVT, AUTO_DETECT_RELEASE_INTERVAL);
		
		break;

	case status_detect_qc20_restore:
		s_status = status_detect_qc30_increase_before;
		SetQC30Mode();
		osal_start_timerEx(Hal_TaskID, AUTO_DETECT_CHECK_EVT, AUTO_DETECT_INTERVAL);
		
		break;
		
	case status_detect_qc30_increase_before:
		qc30VoltageSaved = integer * 10000 + frac;

		TRACE("qc3.0 saved voltage:%d.%d\r\n", integer, frac);
		
		s_status = status_detect_qc30_increase_after;
		IncreaseQC30Voltage();
		osal_start_timerEx(Hal_TaskID, AUTO_DETECT_CHECK_EVT, AUTO_DETECT_INTERVAL);
		
		break;

	case status_detect_qc30_increase_after:
		{
			uint32 voltage = integer * 10000 + frac;

			TRACE("qc3.0 now voltage:%d.%d\r\n", integer, frac);
			if (voltage >=  qc30VoltageSaved + QC30_STEP - QC30_TARGET_VOLTAGE_DELTA
				&& voltage <= qc30VoltageSaved + QC30_STEP + QC30_TARGET_VOLTAGE_DELTA)
			{
				TRACE("qc3.0 supported\r\n");

				if (s_callback != NULL)
				{
					s_callback(detect_type_qc30, true);
				}
			}
			else
			{
				TRACE("qc3.0 not supported\r\n");
				
				if (s_callback != NULL)
				{
					s_callback(detect_type_qc30, false);
				}
			}
			
			s_status = status_detect_release_qc30;
			StopQC20Sniffer();
			osal_start_timerEx(Hal_TaskID, AUTO_DETECT_CHECK_EVT, AUTO_DETECT_RELEASE_INTERVAL);
		}

		break;

	case status_detect_release_qc30:
		s_status = status_detect_apple_21A;
		StartDetectDType(DetectApple21ACallback);
		
		break;
	}
}

