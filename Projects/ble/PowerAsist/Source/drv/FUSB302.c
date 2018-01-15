
#include "comdef.h"
#include "hal_types.h"
#include "hal_mcu.h"

#include "hal_drivers.h"
#include "osal.h"

#include "I2CDevice.h"

#include "FUSB302.h"

#include "npi.h"
#include "Delay.h"

#define FUSB302_ADDRESS   0x44

#define FUSB302_REG_SWITCHES0           0x02
#define FUSB302_REG_SWITCHES0_CC2_PU_EN  (1<<7)
#define FUSB302_REG_SWITCHES0_CC1_PU_EN  (1<<6)
#define FUSB302_REG_SWITCHES0_VCONN_CC2  (1<<5)
#define FUSB302_REG_SWITCHES0_VCONN_CC1  (1<<4)
#define FUSB302_REG_SWITCHES0_MEAS_CC2   (1<<3)
#define FUSB302_REG_SWITCHES0_MEAS_CC1   (1<<2)
#define FUSB302_REG_SWITCHES0_CC2_PD_EN  (1<<1)
#define FUSB302_REG_SWITCHES0_CC1_PD_EN  (1<<0)

#define FUSB302_REG_SWITCHES1           0x03
#define FUSB302_REG_SWITCHES1_POWERROLE  (1<<7)
#define FUSB302_REG_SWITCHES1_SPECREV1   (1<<6)
#define FUSB302_REG_SWITCHES1_SPECREV0   (1<<5)
#define FUSB302_REG_SWITCHES1_DATAROLE   (1<<4)
#define FUSB302_REG_SWITCHES1_AUTO_GCRC  (1<<2)
#define FUSB302_REG_SWITCHES1_TXCC2_EN   (1<<1)
#define FUSB302_REG_SWITCHES1_TXCC1_EN   (1<<0)

#define PD_REVISION                    (1 << 5)

#define FUSB302_REG_MEASURE             0x04
#define FUSB302_REG_MEASURE_VBUS         (1<<6)
#define FUSB302_REG_MEASURE_MDAC_MV(mv)  (((mv)/42) & 0x3f)

#define FUSB302_REG_SLICE               0x05

#define FUSB302_REG_CONTROL0            0x06
#define FUSB302_REG_CONTROL0_TX_FLUSH      (1<<6)
#define FUSB302_REG_CONTROL0_INT_MASK      (1<<5)
#define FUSB302_REG_CONTROL0_HOST_CUR_MASK (3<<2)
#define FUSB302_REG_CONTROL0_HOST_CUR_3A0  (3<<2)
#define FUSB302_REG_CONTROL0_HOST_CUR_1A5  (2<<2)
#define FUSB302_REG_CONTROL0_HOST_CUR_USB  (1<<2)
#define FUSB302_REG_CONTROL0_TX_START      (1<<0)

#define FUSB302_REG_CONTROL1            0x07
#define FUSB302_REG_CONTROL1_ENSOP2DB    (1<<6)
#define FUSB302_REG_CONTROL1_ENSOP1DB    (1<<5)
#define FUSB302_REG_CONTROL1_BIST_MODE2  (1<<4)
#define FUSB302_REG_CONTROL1_RX_FLUSH    (1<<2)
#define FUSB302_REG_CONTROL1_ENSOP2      (1<<1)
#define FUSB302_REG_CONTROL1_ENSOP1      (1<<0)

#define FUSB302_REG_CONTROL2            0x08
/* two-bit field, valid values below */
#define FUSB302_REG_CONTROL2_MODE       (1<<1)
#define FUSB302_REG_CONTROL2_MODE_DFP   (0x3)
#define FUSB302_REG_CONTROL2_MODE_UFP   (0x2)
#define FUSB302_REG_CONTROL2_MODE_DRP   (0x1)
#define FUSB302_REG_CONTROL2_MODE_POS   (1)
#define FUSB302_REG_CONTROL2_TOGGLE     (1<<0)

#define FUSB302_REG_CONTROL3            0x09
#define FUSB302_REG_CONTROL3_SEND_HARDRESET  (1<<6)
#define FUSB302_REG_CONTROL3_BIST_TMODE      (1<<5) /* 302B Only */
#define FUSB302_REG_CONTROL3_AUTO_HARDRESET  (1<<4)
#define FUSB302_REG_CONTROL3_AUTO_SOFTRESET  (1<<3)
/* two-bit field */
#define FUSB302_REG_CONTROL3_N_RETRIES       (1<<1)
#define FUSB302_REG_CONTROL3_N_RETRIES_POS   (1)
#define FUSB302_REG_CONTROL3_N_RETRIES_SIZE  (2)
#define FUSB302_REG_CONTROL3_AUTO_RETRY      (1<<0)

#define FUSB302_REG_MASK               0x0A
#define FUSB302_REG_MASK_VBUSOK        (1<<7)
#define FUSB302_REG_MASK_ACTIVITY      (1<<6)
#define FUSB302_REG_MASK_COMP_CHNG     (1<<5)
#define FUSB302_REG_MASK_CRC_CHK       (1<<4)
#define FUSB302_REG_MASK_ALERT         (1<<3)
#define FUSB302_REG_MASK_WAKE          (1<<2)
#define FUSB302_REG_MASK_COLLISION     (1<<1)
#define FUSB302_REG_MASK_BC_LVL        (1<<0)

#define FUSB302_REG_POWER               0x0B
#define FUSB302_REG_POWER_PWR             (1<<0)  
/* four-bit field */
#define FUSB302_REG_POWER_PWR_LOW      0x1 /* Bandgap + Wake circuitry */
#define FUSB302_REG_POWER_PWR_MEDIUM   0x3 /* LOW + Receiver + Current refs */
#define FUSB302_REG_POWER_PWR_HIGH     0x7 /* MEDIUM + Measure block */
#define FUSB302_REG_POWER_PWR_ALL      0xF /* HIGH + Internal Oscillator */

#define FUSB302_REG_RESET              0x0C
#define FUSB302_REG_RESET_PD_RESET     (1<<1)
#define FUSB302_REG_RESET_SW_RESET     (1<<0)

#define FUSB302_REG_OCPREG              0x0D

#define FUSB302_REG_MASKA               0x0E
#define FUSB302_REG_MASKA_OCP_TEMP     (1<<7)
#define FUSB302_REG_MASKA_TOGDONE      (1<<6)
#define FUSB302_REG_MASKA_SOFTFAIL     (1<<5)
#define FUSB302_REG_MASKA_RETRYFAIL    (1<<4)
#define FUSB302_REG_MASKA_HARDSENT     (1<<3)
#define FUSB302_REG_MASKA_TX_SUCCESS   (1<<2)
#define FUSB302_REG_MASKA_SOFTRESET    (1<<1)
#define FUSB302_REG_MASKA_HARDRESET    (1<<0)

#define FUSB302_REG_MASKB               0x0F
#define FUSB302_REG_MASKB_GCRCSENT     (1<<0)

#define FUSB302_REG_CONTROL4            0x10

#define FUSB302_REG_STATUS0A            0x3C
#define FUSB302_REG_STATUS0A_SOFTFAIL  (1<<5)
#define FUSB302_REG_STATUS0A_RETRYFAIL (1<<4)
#define FUSB302_REG_STATUS0A_POWER     (1<<2) 
/* two-bit field */
#define FUSB302_REG_STATUS0A_RX_SOFT_RESET (1<<1)
#define FUSB302_REG_STATUS0A_RX_HARD_RESEt (1<<0)

#define FUSB302_REG_STATUS1A            0x3D
/* three-bit field, valid values below */
#define FUSB302_REG_STATUS1A_TOGSS     (1<<3)
#define FUSB302_REG_STATUS1A_TOGSS_RUNNING     0x0
#define FUSB302_REG_STATUS1A_TOGSS_SRC1        0x1
#define FUSB302_REG_STATUS1A_TOGSS_SRC2        0x2
#define FUSB302_REG_STATUS1A_TOGSS_SNK1        0x5
#define FUSB302_REG_STATUS1A_TOGSS_SNK2        0x6
#define FUSB302_REG_STATUS1A_TOGSS_AA          0x7
#define FUSB302_REG_STATUS1A_TOGSS_POS         (3)
#define FUSB302_REG_STATUS1A_TOGSS_MASK        (0x7)
#define FUSB302_REG_STATUS1A_RXSOP2DB          (1<<2)
#define FUSB302_REG_STATUS1A_RXSOP1DB          (1<<1)
#define FUSB302_REG_STATUS1A_RXSOP             (1<<0)

#define FUSB302_REG_INTERRUPTA          0x3E
#define FUSB302_REG_INTERRUPTA_OCP_TEMP    (1<<7)
#define FUSB302_REG_INTERRUPTA_TOGDONE     (1<<6)
#define FUSB302_REG_INTERRUPTA_SOFTFAIL    (1<<5)
#define FUSB302_REG_INTERRUPTA_RETRYFAIL   (1<<4)
#define FUSB302_REG_INTERRUPTA_HARDSENT    (1<<3)
#define FUSB302_REG_INTERRUPTA_TX_SUCCESS  (1<<2)
#define FUSB302_REG_INTERRUPTA_SOFTRESET   (1<<1)
#define FUSB302_REG_INTERRUPTA_HARDRESET   (1<<0)

#define FUSB302_REG_INTERRUPTB            0x3F
#define FUSB302_REG_INTERRUPTB_GCRCSENT   (1<<0)

#define FUSB302_REG_STATUS0             0x40
#define FUSB302_REG_STATUS0_VBUSOK     (1<<7)
#define FUSB302_REG_STATUS0_ACTIVITY   (1<<6)
#define FUSB302_REG_STATUS0_COMP       (1<<5)
#define FUSB302_REG_STATUS0_CRC_CHK    (1<<4)
#define FUSB302_REG_STATUS0_ALERT      (1<<3)
#define FUSB302_REG_STATUS0_WAKE       (1<<2)
#define FUSB302_REG_STATUS0_BC_LVL1    (1<<1) /* two-bit field */
#define FUSB302_REG_STATUS0_BC_LVL0    (1<<0) /* two-bit field */

#define FUSB302_REG_STATUS1             0x41
#define FUSB302_REG_STATUS1_RXSOP2     (1<<7)
#define FUSB302_REG_STATUS1_RXSOP1     (1<<6)
#define FUSB302_REG_STATUS1_RX_EMPTY   (1<<5)
#define FUSB302_REG_STATUS1_RX_FULL    (1<<4)
#define FUSB302_REG_STATUS1_TX_EMPTY   (1<<3)
#define FUSB302_REG_STATUS1_TX_FULL    (1<<2)

#define FUSB302_REG_INTERRUPT           0x42
#define FUSB302_REG_INTERRUPT_VBUSOK   (1<<7)
#define FUSB302_REG_INTERRUPT_ACTIVITY (1<<6)
#define FUSB302_REG_INTERRUPT_COMP_CHNG (1<<5)
#define FUSB302_REG_INTERRUPT_CRC_CHK  (1<<4)
#define FUSB302_REG_INTERRUPT_ALERT    (1<<3)
#define FUSB302_REG_INTERRUPT_WAKE     (1<<2)
#define FUSB302_REG_INTERRUPT_COLLISION (1<<1)
#define FUSB302_REG_INTERRUPT_BC_LVL   (1<<0)

#define FUSB302_REG_FIFOS               0x43

#define FUSB_TKN_TXON		0xA1
#define FUSB_TKN_SYNC1		0x12
#define FUSB_TKN_SYNC2		0x13
#define FUSB_TKN_SYNC3		0x1B
#define FUSB_TKN_RST1		0x15
#define FUSB_TKN_RST2		0x16
#define FUSB_TKN_PACKSYM	0x80
#define FUSB_TKN_JAMCRC		0xFF
#define FUSB_TKN_EOP		0x14
#define FUSB_TKN_TXOFF		0xFE

/* Default retry count for transmitting */
#define PD_RETRY_COUNT      3

#define FUSB302_INT_RISING_EDGE   0
#define FUSB302_INT_FALLING_EDGE  1

/* CPU port interrupt */
#define FUSB302_INT_CPU_PORT_1_IF  P1IF

/* int is at P1.2 */
#define FUSB302_INT_PORT   P1
#define FUSB302_INT_BIT    BV(2)
#define FUSB302_INT_SEL    P1SEL
#define FUSB302_INT_DIR    P1DIR

/* edge interrupt */
#define FUSB302_INT_EDGEBIT  BV(1)
#define FUSB302_INT_EDGE     FUSB302_INT_FALLING_EDGE

/* interrupts */
#define FUSB302_INT_IEN      IEN2  /* CPU interrupt mask register */
#define FUSB302_INT_IENBIT   BV(4) /* Mask bit for all of Port_1 */
#define FUSB302_INT_ICTL     P1IEN /* Port Interrupt Control register */
#define FUSB302_INT_ICTLBIT  BV(2) /* P0IEN - P1.2 enable/disable bit */
#define FUSB302_INT_PXIFG    P1IFG /* Interrupt flag at source */

static CC_PARITY s_parity = parity_none;

static void (*s_recvCallback)(uint16 header, const uint32 *pdo, uint8 cnt) = NULL;

bool DetectFUSB302()
{
	return DetectI2CDevice(FUSB302_ADDRESS);
}

static bool WriteFUSB302Byte(uint8 addr, uint8 dat)
{
	return WriteI2CDeviceData(FUSB302_ADDRESS, &addr, sizeof(addr), &dat, sizeof(dat));
}

static bool ReadFUSB302Byte(uint8 addr, uint8 *pData)
{
	return ReadI2CDeviceData(FUSB302_ADDRESS, &addr, sizeof(addr), pData, sizeof(*pData), NULL);
}

static bool WriteFUSB302Data(uint8 addr, const uint8 *pData, uint16 len)
{
	return WriteI2CDeviceData(FUSB302_ADDRESS, &addr, sizeof(addr), pData, len);
}

static bool ReadFUSB302Data(uint8 addr, uint8 *pData, uint16 len, uint16 *pRetLen)
{
	return ReadI2CDeviceData(FUSB302_ADDRESS, &addr, sizeof(addr), pData, len, pRetLen);
}

static void FlushRxFifo()
{
	WriteFUSB302Byte(FUSB302_REG_CONTROL1, FUSB302_REG_CONTROL1_RX_FLUSH);
}

static void FlushTxFifo()
{
	uint8 reg;

	ReadFUSB302Byte(FUSB302_REG_CONTROL0, &reg);
	reg |= FUSB302_REG_CONTROL0_TX_FLUSH;
	WriteFUSB302Byte(FUSB302_REG_CONTROL0, reg);
}

static void SendMessage(uint16 header, const uint32 *pdo, uint8 cnt)
{
	//max len 40
	uint8 buf[40];

	uint8 pos = 0;
	
	uint16 len = cnt * sizeof(uint32) + sizeof(uint16);

	buf[pos++] = FUSB_TKN_SYNC1;
	buf[pos++] = FUSB_TKN_SYNC1;
	buf[pos++] = FUSB_TKN_SYNC1;
	buf[pos++] = FUSB_TKN_SYNC2;
	buf[pos++] = FUSB_TKN_PACKSYM | (len & 0x1F);
	buf[pos++] = header & 0xFF;
	buf[pos++] = header >> 8;

	len -= 2;
	osal_memcpy(&buf[pos], pdo, len);
	pos += len;

	buf[pos++] = FUSB_TKN_JAMCRC;
	buf[pos++] = FUSB_TKN_EOP;
	buf[pos++] = FUSB_TKN_TXOFF;
	buf[pos++] = FUSB_TKN_TXON;

	WriteFUSB302Data(FUSB302_REG_FIFOS, buf, pos);
}

void PDTransmit(uint16 header, const uint32 *data, uint8 cnt)
{
	WriteFUSB302Byte(FUSB302_REG_POWER, FUSB302_REG_POWER_PWR_ALL);

	FlushRxFifo();
	
	FlushTxFifo();

	SendMessage(header, data, cnt);

	WriteFUSB302Byte(FUSB302_REG_POWER, FUSB302_REG_POWER_PWR_HIGH);
}

void PDRegisterRecvCallback(void (*callback)(uint16, const uint32 *, uint8))
{
	s_recvCallback = callback;
}

static void EnableFUSB302Interrupt(bool enable)
{
	FUSB302_INT_PXIFG &= ~(FUSB302_INT_BIT);
	
	if (enable)
	{
		FUSB302_INT_ICTL |= FUSB302_INT_ICTLBIT;
	}
	else
	{
		FUSB302_INT_ICTL &= ~(FUSB302_INT_ICTLBIT);
	}
}

void ProcessFUSB302Interrupt(void)
{
	uint8 reg;

	ReadFUSB302Byte(FUSB302_REG_INTERRUPT, &reg);

	if (reg & FUSB302_REG_INTERRUPT_CRC_CHK)
	{
		uint8 buf[40];
		
		//read token(1B) + header(2B)
		if (ReadFUSB302Data(FUSB302_REG_FIFOS, buf, 3, NULL))
		{		
			uint16 header = *(uint16 *)(buf + 1);
			uint16 cnt = PD_HEADER_CNT(header);
			//read data + crc(4B)
			ReadFUSB302Data(FUSB302_REG_FIFOS, buf, (cnt + 1) * 4, NULL);

			/*
			FlushRxFifo();
			*/
			
			if (s_recvCallback != NULL)
			{
				s_recvCallback(header, (uint32 *)buf, cnt);
			}
		}
	}

	EnableFUSB302Interrupt(true);
}

HAL_ISR_FUNCTION(intPort1Isr, P1INT_VECTOR)
{
	HAL_ENTER_ISR();

	if (FUSB302_INT_PXIFG & FUSB302_INT_BIT)  
	{
		uint8 *ptr = osal_msg_allocate(sizeof(bool));
		if (ptr)
		{
			*ptr = FUSB302_MSG_ID;
			osal_msg_send(Hal_TaskID, ptr);
		}

		//clear
		FUSB302_INT_PXIFG &= ~(FUSB302_INT_BIT);
	}
	
	/*
		Clear the CPU interrupt flag for Port_1
		PxIFG has to be cleared before PxIF
  	*/
	FUSB302_INT_CPU_PORT_1_IF = 0;

	CLEAR_SLEEP_MODE();

	HAL_EXIT_ISR();
}

uint8 GetFUSB302DeviceID()
{
	uint8 reg;
	ReadFUSB302Byte(FUSB302_REG_DEVICEID, &reg);

	return reg;
}

static void InitFUSB302Gpio()
{
  	FUSB302_INT_SEL &= ~(FUSB302_INT_BIT);    /* Set pin function to GPIO */
  	FUSB302_INT_DIR &= ~(FUSB302_INT_BIT);    /* Set pin direction to Input */

  	/* Rising/Falling edge configuratinn */
    PICTL &= ~(FUSB302_INT_EDGEBIT);    /* Clear the edge bit */
    /* For falling edge, the bit must be set. */
  #if (FUSB302_INT_EDGE == FUSB302_INT_FALLING_EDGE)
    PICTL |= FUSB302_INT_EDGEBIT;
  #endif

	FUSB302_INT_ICTL |= FUSB302_INT_ICTLBIT;
	
	FUSB302_INT_IEN |= FUSB302_INT_IENBIT;
}

void InitFUSB302()
{
	InitFUSB302Gpio();

	//reset
	WriteFUSB302Byte(FUSB302_REG_RESET, FUSB302_REG_RESET_PD_RESET | FUSB302_REG_RESET_SW_RESET);

	//Turn on the power
	WriteFUSB302Byte(FUSB302_REG_POWER, FUSB302_REG_POWER_PWR_HIGH);
}

CC_PARITY GetCCParity()
{
	return s_parity;
}

void ConfigFUSB302()
{
	uint8 reg;

	s_parity = parity_none;
	
	WriteFUSB302Byte(FUSB302_REG_RESET, FUSB302_REG_RESET_PD_RESET | FUSB302_REG_RESET_SW_RESET);

	DelayMs(1);

	//power enable
	WriteFUSB302Byte(FUSB302_REG_POWER, FUSB302_REG_POWER_PWR_ALL);

	WriteFUSB302Byte(FUSB302_REG_SWITCHES0, FUSB302_REG_SWITCHES0_CC1_PD_EN 
											| FUSB302_REG_SWITCHES0_CC2_PD_EN);
											
	WriteFUSB302Byte(FUSB302_REG_SWITCHES0, FUSB302_REG_SWITCHES0_CC1_PD_EN 
											| FUSB302_REG_SWITCHES0_CC2_PD_EN
											| FUSB302_REG_SWITCHES0_MEAS_CC1);

	ReadFUSB302Byte(FUSB302_REG_STATUS0, &reg);
	TRACE("0x%02X:0x%02X\r\n", FUSB302_REG_STATUS0, reg);
	if (reg & (FUSB302_REG_STATUS0_BC_LVL1 | FUSB302_REG_STATUS0_BC_LVL0))
	{
		s_parity = parity_cc1;
	}
	
	ReadFUSB302Byte(FUSB302_REG_STATUS0, &reg);
	TRACE("0x%02X:0x%02X\r\n", FUSB302_REG_STATUS0, reg);
	if (reg & (FUSB302_REG_STATUS0_BC_LVL1 | FUSB302_REG_STATUS0_BC_LVL0))
	{
		s_parity = parity_cc1;
	}

	if (s_parity == parity_none)
	{
		WriteFUSB302Byte(FUSB302_REG_SWITCHES0, FUSB302_REG_SWITCHES0_CC1_PD_EN 
											| FUSB302_REG_SWITCHES0_CC2_PD_EN);
											
		WriteFUSB302Byte(FUSB302_REG_SWITCHES0, FUSB302_REG_SWITCHES0_CC1_PD_EN 
											| FUSB302_REG_SWITCHES0_CC2_PD_EN
											| FUSB302_REG_SWITCHES0_MEAS_CC2);
											
		ReadFUSB302Byte(FUSB302_REG_STATUS0, &reg);
		TRACE("0x%02X:0x%02X\r\n", FUSB302_REG_STATUS0, reg);
		if (reg & (FUSB302_REG_STATUS0_BC_LVL1 | FUSB302_REG_STATUS0_BC_LVL0))
		{
			s_parity = parity_cc2;
		}

		ReadFUSB302Byte(FUSB302_REG_STATUS0, &reg);
		TRACE("0x%02X:0x%02X\r\n", FUSB302_REG_STATUS0, reg);
		if (reg & (FUSB302_REG_STATUS0_BC_LVL1 | FUSB302_REG_STATUS0_BC_LVL0))
		{
			s_parity = parity_cc2;
		}
	}
	
	WriteFUSB302Byte(FUSB302_REG_RESET, FUSB302_REG_RESET_PD_RESET | FUSB302_REG_RESET_SW_RESET);

	DelayMs(1);

	//retry count
	WriteFUSB302Byte(FUSB302_REG_CONTROL3, (PD_RETRY_COUNT << FUSB302_REG_CONTROL3_N_RETRIES_POS) 
											| FUSB302_REG_CONTROL3_AUTO_RETRY);

	WriteFUSB302Byte(FUSB302_REG_MASKA, 0xFF);
	WriteFUSB302Byte(FUSB302_REG_MASKB, FUSB302_REG_MASKB_GCRCSENT);

	reg = 0xFF;
	reg &= ~FUSB302_REG_MASK_CRC_CHK;
	WriteFUSB302Byte(FUSB302_REG_MASK, reg);

	if (s_parity == parity_cc1)
	{
		WriteFUSB302Byte(FUSB302_REG_SWITCHES0, FUSB302_REG_SWITCHES0_CC1_PD_EN 
											| FUSB302_REG_SWITCHES0_CC2_PD_EN
											| FUSB302_REG_SWITCHES0_MEAS_CC1);
											
		WriteFUSB302Byte(FUSB302_REG_SWITCHES1, PD_REVISION 
						| FUSB302_REG_SWITCHES1_TXCC1_EN
						| FUSB302_REG_SWITCHES1_AUTO_GCRC);
	}
	else if (s_parity == parity_cc2)
	{
		WriteFUSB302Byte(FUSB302_REG_SWITCHES0, FUSB302_REG_SWITCHES0_CC1_PD_EN 
											| FUSB302_REG_SWITCHES0_CC2_PD_EN
											| FUSB302_REG_SWITCHES0_MEAS_CC2);

		WriteFUSB302Byte(FUSB302_REG_SWITCHES1, PD_REVISION 
						| FUSB302_REG_SWITCHES1_TXCC2_EN
						| FUSB302_REG_SWITCHES1_AUTO_GCRC);
	}

	WriteFUSB302Byte(FUSB302_REG_CONTROL0, 0x00);
	
	WriteFUSB302Byte(FUSB302_REG_RESET, FUSB302_REG_RESET_PD_RESET);
	
	WriteFUSB302Byte(FUSB302_REG_POWER, FUSB302_REG_POWER_PWR_ALL);

	ReadFUSB302Byte(FUSB302_REG_INTERRUPTA, &reg);
	//TRACE("0x%02X:0x%02X\r\n", FUSB302_REG_INTERRUPTA, reg);

	ReadFUSB302Byte(FUSB302_REG_INTERRUPTB, &reg);
	//TRACE("0x%02X:0x%02X\r\n", FUSB302_REG_INTERRUPTB, reg);

	ReadFUSB302Byte(FUSB302_REG_INTERRUPT, &reg);
	//TRACE("0x%02X:0x%02X\r\n", FUSB302_REG_INTERRUPT, reg);
}

void ResetFUSB302()
{
	WriteFUSB302Byte(FUSB302_REG_POWER, FUSB302_REG_POWER_PWR_ALL);

	WriteFUSB302Byte(FUSB302_REG_CONTROL3, FUSB302_REG_CONTROL3_SEND_HARDRESET 
											| (PD_RETRY_COUNT << FUSB302_REG_CONTROL3_N_RETRIES_POS) 
											| FUSB302_REG_CONTROL3_AUTO_RETRY);

	s_recvCallback = NULL;
}

