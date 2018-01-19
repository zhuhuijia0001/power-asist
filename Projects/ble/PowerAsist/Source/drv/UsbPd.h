
#ifndef _USBPD_H_
#define _USBPD_H_

/* Protocol revision */
#define PD_REV10 0
#define PD_REV20 1
/* Power role */
#define PD_ROLE_SINK   0
#define PD_ROLE_SOURCE 1
/* Data role */
#define PD_ROLE_UFP    0
#define PD_ROLE_DFP    1
/* Vconn role */
#define PD_ROLE_VCONN_OFF 0

#define PD_ROLE_VCONN_ON  1

/* build message header */
#define PD_HEADER(type, prole, drole, id, cnt) \
					((type) | (PD_REV20 << 6) | \
					((drole) << 5) | ((prole) << 8) | \
					((id) << 9) | ((cnt) << 12))

/* USB PD Control Message Types */
#define CONTROLMESSAGE		0
#define CMT_GOODCRC		    1
#define CMT_GOTOMIN		    2
#define CMT_ACCEPT		    3
#define CMT_REJECT		    4
#define CMT_PING		    5
#define CMT_PS_RDY		    6
#define CMT_GETSOURCECAP	7
#define CMT_GETSINKCAP		8
#define CMT_DR_SWAP		    9
#define CMT_PR_SWAP		    10
#define CMT_VCONN_SWAP		11
#define CMT_WAIT		    12
#define CMT_SOFTRESET		13

/* USB PD Data Message Types */
#define DATAMESSAGE		        1
#define DMT_SOURCECAPABILITIES	1
#define DMT_REQUEST		        2
#define DMT_BIST		        3
#define DMT_SINKCAPABILITIES	4
#define DMT_VENDERDEFINED	    15

#define CAP_POWER_TYPE(PDO)		    ((PDO >> 30) & 3)
#define CAP_FPDO_VOLTAGE(PDO)		((PDO >> 10) & 0x3ff)
#define CAP_VPDO_VOLTAGE(PDO)		((PDO >> 20) & 0x3ff)
#define CAP_FPDO_CURRENT(PDO)		((PDO >> 0) & 0x3ff)
#define CAP_VPDO_CURRENT(PDO)		((PDO >> 0) & 0x3ff)

/* RDO : Request Data Object */
#define RDO_OBJ_POS(n)             (((n) & 0x7l) << 28)
#define RDO_POS(rdo)               (((rdo) >> 28l) & 0x7)
#define RDO_GIVE_BACK              (1l << 27)
#define RDO_CAP_MISMATCH           (1l << 26)
#define RDO_COMM_CAP               (1l << 25)
#define RDO_NO_SUSPEND             (1l << 24)
#define RDO_FIXED_VAR_OP_CURR(ma)  ((((ma) / 10) & 0x3FFl) << 10)
#define RDO_FIXED_VAR_MAX_CURR(ma) ((((ma) / 10) & 0x3FFl) << 0)
#define RDO_BATT_OP_POWER(mw)      ((((mw) / 250) & 0x3FFl) << 10)
#define RDO_BATT_MAX_POWER(mw)     ((((mw) / 250) & 0x3FFl) << 10)
#define RDO_FIXED(n, op_ma, max_ma, flags) \
  				(RDO_OBJ_POS(n) | (flags) | \
				RDO_FIXED_VAR_OP_CURR(op_ma) | \
				RDO_FIXED_VAR_MAX_CURR(max_ma))

#define RDO_BATT(n, op_mw, max_mw, flags) \
  				(RDO_OBJ_POS(n) | (flags) | \
				RDO_BATT_OP_POWER(op_mw) | \
				RDO_BATT_MAX_POWER(max_mw))

#define MAX_CAPABILITIES    7

typedef struct 
{
	uint16 voltage; //voltage
	
	uint16 opMa;    //operation mA

	uint16 maxMa;   //max mA
} CapabilityStruct;

typedef struct
{
	CapabilityStruct caps[MAX_CAPABILITIES];

	uint8 cnt;
} CapabilityList;

#define GET_CAPS_OK        1
#define GET_CAPS_FAILED    2
#define REQUEST_ACCEPT     3
#define REQUEST_PS_RDY     4
#define REQUEST_REJECT     5 
#define REQUEST_FAILED     6

extern void EnterUsbPd(void (*callback)(uint8 res, const CapabilityList *list));

extern void ExitUsbPd();

extern void ProcessUsbPdTimeout();

extern void PdGetSourceCap(void (*callback)(uint8 res, const CapabilityList *list));

extern bool PdRequest(uint8 pos, void (*callback)(uint8 requestedVoltage, uint8 res));

extern const CapabilityList *GetPDCaps();

#endif
