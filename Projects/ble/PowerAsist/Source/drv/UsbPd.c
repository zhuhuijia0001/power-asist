#include "comdef.h"
#include "hal_types.h"

#include "hal_drivers.h"
#include "osal.h"

#include "FUSB302.h"

#include "UsbPd.h"

#include "npi.h"

static uint8 s_msg_id = 0;

static CapabilityList s_capsList;

static void (*s_getCapsCallback)(uint8 res, const CapabilityList *list) = NULL;

static void (*s_requestCallback)(uint8 res) = NULL;

//usb pd status
typedef enum
{
	pd_status_idle = 0,

	pd_status_hard_reset,

	pd_status_wait_src_caps,

	pd_status_wait_accept,

	pd_status_wait_ps_rdy,
} PD_STATUS;

static PD_STATUS s_pd_status = pd_status_idle;

static void IncreaseMsgID()
{
	s_msg_id = (s_msg_id + 1) & 0x07;
}

static void ReceiveCallback(uint16 header, const uint32 *pdo, uint8 cnt)
{
	uint8 type = PD_HEADER_TYPE(header);
	
#ifdef DEBUG_PRINT
	uint8 msgId = PD_HEADER_ID(header);
	TRACE("msg id:%d\r\n", msgId);
#endif
	
	if (cnt == 0)
	{
		//control message
		switch (type)
		{
		case CMT_GOODCRC:
			IncreaseMsgID();

			break;
				
		case CMT_ACCEPT:
			TRACE("Accept\r\n");

			s_pd_status = pd_status_wait_ps_rdy;
			osal_stop_timerEx(Hal_TaskID, USB_PD_EVENT);

			if (s_requestCallback != NULL)
			{
				s_requestCallback(REQUEST_ACCEPT);
			}
			//osal_start_timerEx(Hal_TaskID, USB_PD_EVENT, 500);

			break;
	
		case CMT_PS_RDY:
			TRACE("PS_RDY\r\n");

			osal_stop_timerEx(Hal_TaskID, USB_PD_EVENT);
			
			if (s_requestCallback != NULL)
			{
				s_requestCallback(REQUEST_PS_RDY);
			}

			s_pd_status = pd_status_idle;

			break;
		
		case CMT_REJECT:
			TRACE("Reject\r\n");

			osal_stop_timerEx(Hal_TaskID, USB_PD_EVENT);
			
			if (s_requestCallback != NULL)
			{
				s_requestCallback(REQUEST_REJECT);
			}

			s_pd_status = pd_status_idle;

			break;
		}
	}
	else
	{
		//data message
		if (type == DMT_SOURCECAPABILITIES)
		{
			int i;

			osal_stop_timerEx(Hal_TaskID, USB_PD_EVENT);
			
			if (cnt > MAX_CAPABILITIES)
			{
				cnt = MAX_CAPABILITIES;
			}
			
			for (i = 0; i < cnt; i++)
			{
				s_capsList.caps[i].voltage = CAP_FPDO_VOLTAGE(*(pdo + i)) * 50;
				s_capsList.caps[i].maxMa = CAP_FPDO_CURRENT(*(pdo + i)) * 10;
				s_capsList.caps[i].opMa = s_capsList.caps[i].maxMa;
			}

			s_capsList.cnt = cnt;
			
			if (s_getCapsCallback != NULL)
			{
				s_getCapsCallback(GET_CAPS_OK, &s_capsList);
			}
		}			
	}
}

static void SendControl(uint8 type)
{
	uint16 header = PD_HEADER(type, PD_ROLE_SINK, PD_ROLE_UFP, s_msg_id, 0);
	
	PDTransmit(header, NULL, 0);
}

static void SendRequest(uint32 rdo)
{
	uint16 header = PD_HEADER(DMT_REQUEST, PD_ROLE_SINK, PD_ROLE_UFP, s_msg_id, 1);

	PDTransmit(header, &rdo, 1);
}

void PdGetSourceCap(void (*callback)(uint8 res, const CapabilityList *list))
{
	s_getCapsCallback = callback;
	SendControl(CMT_GETSOURCECAP);

	s_pd_status = pd_status_wait_src_caps;
	osal_start_timerEx(Hal_TaskID, USB_PD_EVENT, 200);
}

void PdRequest(uint8 pos, void (*callback)(uint8 res))
{
	if (pos < 1 || pos > MAX_CAPABILITIES)
	{
		callback(REQUEST_FAILED);
		
		return;
	}

	s_requestCallback = callback;
	uint32 rdo = RDO_FIXED(pos, s_capsList.caps[pos - 1].opMa, s_capsList.caps[pos - 1].maxMa, 0);
	SendRequest(rdo);

	s_pd_status = pd_status_wait_accept;
	osal_start_timerEx(Hal_TaskID, USB_PD_EVENT, 500);
}

void EnterUsbPd(void (*callback)(uint8 res, const CapabilityList *list))
{
	s_msg_id = 0;
	
	PDRegisterRecvCallback(ReceiveCallback);
	
	s_getCapsCallback = callback;
	
	ConfigFUSB302();
	
	ResetFUSB302();

	s_pd_status = pd_status_hard_reset;
	osal_start_timerEx(Hal_TaskID, USB_PD_EVENT, 1000);
}

void ProcessUsbPdTimeout()
{	
	if (s_pd_status == pd_status_hard_reset)
	{
		s_msg_id = 0;
		
		PDRegisterRecvCallback(ReceiveCallback);

		ConfigFUSB302();

		PdGetSourceCap(s_getCapsCallback);
	}
	else if (s_pd_status == pd_status_wait_src_caps)
	{
		if (s_getCapsCallback != NULL)
		{
			s_getCapsCallback(GET_CAPS_FAILED, NULL);
		}

		s_pd_status = pd_status_idle;
	}
	else if (s_pd_status == pd_status_wait_accept
			|| s_pd_status == pd_status_wait_ps_rdy)
	{
		if (s_requestCallback != NULL)
		{
			s_requestCallback(REQUEST_FAILED);
		}

		s_pd_status = pd_status_idle;
	}
}

void ExitUsbPd()
{
	ResetFUSB302();

	s_pd_status = pd_status_idle;
}

const CapabilityList *GetPDCaps()
{
	return &s_capsList;
}

