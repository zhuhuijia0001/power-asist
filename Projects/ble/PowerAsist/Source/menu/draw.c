#include <stdio.h>
#include "hal_types.h"

#include "draw.h"

#include "GUI.h"

#include "Strings.h"
#include "Parameter.h"

//menu color
#define BACKGROUND_COLOR       BLACK

#define NORMAL_COLOR           ORANGE
#define SELECTED_COLOR         WHITE
#define EDIT_COLOR             MAGENTA
#define PRESSED_COLOR          WHITE

//main menu
#define MAIN_UNIT_LEFT         110

#define MAIN_VOLTAGE_LEFT      2
#define MAIN_VOLTAGE_TOP       5

#define MAIN_CURRENT_LEFT      2
#define MAIN_CURRENT_TOP       35

#define MAIN_POWER_LEFT        2
#define MAIN_POWER_TOP         65

#define MAIN_DATE_LEFT         45
#define MAIN_DATE_TOP          95

#define MAIN_TIME_LEFT         45
#define MAIN_TIME_TOP          112

void DrawMainMenu()
{
	DrawString(font_24, MAIN_UNIT_LEFT, MAIN_VOLTAGE_TOP, "V", CYAN, BACKGROUND_COLOR);

	DrawString(font_24, MAIN_UNIT_LEFT, MAIN_CURRENT_TOP, "A", CYAN, BACKGROUND_COLOR);

	DrawString(font_24, MAIN_UNIT_LEFT, MAIN_POWER_TOP, "W", CYAN, BACKGROUND_COLOR);
}

static void DrawVBusValue(FONT font, uint16 x, uint16 y, uint8 dec, uint16 frac, uint16 fc)
{
	char buf[10];
	if (dec < 10)
	{
		sprintf(buf, "% d.%04d", dec, frac);
	}
	else
	{
		sprintf(buf, "%2d.%04d", dec, frac);
	}
	
	DrawString(font, x, y, (uint8 *)buf, fc, BACKGROUND_COLOR);
}

void DrawMainVoltage(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_24, MAIN_VOLTAGE_LEFT, MAIN_VOLTAGE_TOP, dec, frac, CYAN);
}

void DrawMainCurrent(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_24, MAIN_CURRENT_LEFT, MAIN_CURRENT_TOP, dec, frac, CYAN);
}

void DrawMainPower(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_24, MAIN_POWER_LEFT, MAIN_POWER_TOP, dec, frac, CYAN);
}

void DrawDateTime(const TimeStruct *time)
{
	char buf[11];
	sprintf(buf, "%04d-%02d-%02d", time->year, time->month, time->day);
	DrawString(font_16, MAIN_DATE_LEFT, MAIN_DATE_TOP, (uint8 *)buf, CYAN, BACKGROUND_COLOR);
	
	sprintf(buf, "%02d:%02d:%02d", time->hour, time->minute, time->second);
	DrawString(font_16, MAIN_TIME_LEFT, MAIN_TIME_TOP, (uint8 *)buf, CYAN, BACKGROUND_COLOR);
}

//detail menu
#define DETAIL_DP_TOP            0
#define DETAIL_DP_TITLE_LEFT     0
#define DETAIL_DP_VAL_LEFT       16
#define DETAIL_DP_UNIT_LEFT      48

#define DETAIL_DM_TOP            16
#define DETAIL_DM_TITLE_LEFT     0
#define DETAIL_DM_VAL_LEFT       16
#define DETAIL_DM_UNIT_LEFT      48

#define DETAIL_TYPE_TITLE_LEFT   60
#define DETAIL_TYPE_TITLE_TOP    DETAIL_DP_TOP

#define DETAIL_TYPE_LEFT         60
#define DETAIL_TYPE_TOP          DETAIL_DM_TOP

#define DETAIL_TYPE_VOLTAGE_LEFT 102

#define DETAIL_UNIT_LEFT         105

#define DETAIL_VOLTAGE_LEFT      3
#define DETAIL_VOLTAGE_TOP       32

#define DETAIL_CURRENT_LEFT      3
#define DETAIL_CURRENT_TOP       52

#define DETAIL_POWER_LEFT        3
#define DETAIL_POWER_TOP         72

#define DETAIL_WH_LEFT           45
#define DETAIL_WH_TOP            92
#define DETAIL_WH_UNIT_LEFT      100

#define DETAIL_AH_LEFT           45
#define DETAIL_AH_TOP            108
#define DETAIL_AH_UNIT_LEFT      100

void DrawDetailSniffer(SnifferStatus sniffer, uint8 voltage)
{
	const uint8 *p = NULL;

	uint8 hasVoltage = 0;
	
	if (sniffer == SNIFFER_QC_20)
	{
		p = str_qc20;

		hasVoltage = 1;
	}
	else if (sniffer == SNIFFER_QC_30)
	{
		p = str_qc30;
	}
	else if (sniffer == SNIFFER_PD)
	{
		p = str_pd;

		hasVoltage = 1;
	}
	else
	{
		p = str_na;
	}
	
	DrawString(font_16, DETAIL_TYPE_LEFT, DETAIL_TYPE_TOP, p, GREEN, BACKGROUND_COLOR);

	if (hasVoltage)
	{
		char buf[5];
		sprintf(buf, "%dV", voltage);

		DrawString(font_16, DETAIL_TYPE_VOLTAGE_LEFT, DETAIL_TYPE_TOP, (uint8 *)buf, GREEN, BACKGROUND_COLOR);
	}
}

void DrawDetailMenu()
{
	DrawString(font_16, DETAIL_DP_TITLE_LEFT, DETAIL_DP_TOP, "D+:", YELLOW, BACKGROUND_COLOR);
	DrawString(font_16, DETAIL_DP_UNIT_LEFT, DETAIL_DP_TOP, "V", YELLOW, BACKGROUND_COLOR);
	
	DrawString(font_16, DETAIL_DM_TITLE_LEFT, DETAIL_DM_TOP, "D-:", YELLOW, BACKGROUND_COLOR);
	DrawString(font_16, DETAIL_DM_UNIT_LEFT, DETAIL_DM_TOP, "V", YELLOW, BACKGROUND_COLOR);
	
	DrawString(font_16, DETAIL_TYPE_TITLE_LEFT, DETAIL_TYPE_TITLE_TOP, "TYPE:", GREEN, BACKGROUND_COLOR);
	
	//measure
	DrawString(font_20, DETAIL_UNIT_LEFT, DETAIL_VOLTAGE_TOP, "V", CYAN, BACKGROUND_COLOR);

	DrawString(font_20, DETAIL_UNIT_LEFT, DETAIL_CURRENT_TOP, "A", CYAN, BACKGROUND_COLOR);

	DrawString(font_20, DETAIL_UNIT_LEFT, DETAIL_POWER_TOP, "W", CYAN, BACKGROUND_COLOR);

	//Wh
	DrawString(font_16, DETAIL_WH_UNIT_LEFT, DETAIL_WH_TOP, "Wh", GREEN, BACKGROUND_COLOR);
	
	//Ah
	DrawString(font_16, DETAIL_AH_UNIT_LEFT, DETAIL_AH_TOP, "Ah", GREEN, BACKGROUND_COLOR);
}

static void DrawDetailDpDm(uint16 x, uint16 y, uint8 dec, uint16 frac, uint16 fc)
{
	char buf[6];
	sprintf(buf, "%d.%02d", dec, frac);
	DrawString(font_16, x, y, (uint8 *)buf, fc, BACKGROUND_COLOR);
}

void DrawDetailDp(uint8 dec, uint16 frac)
{
	DrawDetailDpDm(DETAIL_DP_VAL_LEFT, DETAIL_DP_TOP, dec, frac, YELLOW);
}

void DrawDetailDm(uint8 dec, uint16 frac)
{
	DrawDetailDpDm(DETAIL_DM_VAL_LEFT, DETAIL_DM_TOP, dec, frac, YELLOW);
}

void DrawDetailVoltage(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, DETAIL_VOLTAGE_LEFT, DETAIL_VOLTAGE_TOP, dec, frac, CYAN);
}

void DrawDetailCurrent(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, DETAIL_CURRENT_LEFT, DETAIL_CURRENT_TOP, dec, frac, CYAN);
}

void DrawDetailPower(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, DETAIL_POWER_LEFT, DETAIL_POWER_TOP, dec, frac, CYAN);
}

static void DrawDetailAhWh(uint16 x, uint16 y, uint16 dec, uint16 frac, uint16 fc)
{
	char buf[10];
	sprintf(buf, "%d.%04d", dec, frac);

	DrawString(font_16, x, y, (uint8 *)buf, fc, BACKGROUND_COLOR);
}

void DrawDetailWh(uint16 dec, uint16 frac)
{
	DrawDetailAhWh(DETAIL_WH_LEFT, DETAIL_WH_TOP, dec, frac, GREEN);
}

void DrawDetailAh(uint16 dec, uint16 frac)
{
	DrawDetailAhWh(DETAIL_AH_LEFT, DETAIL_AH_TOP, dec, frac, GREEN);
}

//sniffer menu
#define SNIFFER_TITLE_LEFT        8
#define SNIFFER_TITLE_TOP         10

#define SNIFFER_DELTA             1

#define SNIFFER_QC20_LEFT         20
#define SNIFFER_QC20_TOP          35

#define SNIFFER_QC30_LEFT         SNIFFER_QC20_LEFT
#define SNIFFER_QC30_TOP          53

#define SNIFFER_USB_PD_LEFT       SNIFFER_QC20_LEFT
#define SNIFFER_USB_PD_TOP        71

#define SNIFFER_AUTO_DETECT_LEFT  SNIFFER_QC20_LEFT
#define SNIFFER_AUTO_DETECT_TOP   89

#define SNIFFER_BACK_LEFT         80
#define SNIFFER_BACK_TOP          107

void DrawSnifferMenu()
{
	//title
	DrawString(font_20, SNIFFER_TITLE_LEFT, SNIFFER_TITLE_TOP, str_sniffer, CYAN, BACKGROUND_COLOR);

	DrawString(font_16, SNIFFER_QC20_LEFT, SNIFFER_QC20_TOP, str_qc20, NORMAL_COLOR, BACKGROUND_COLOR);
	DrawString(font_16, SNIFFER_QC30_LEFT, SNIFFER_QC30_TOP, str_qc30, NORMAL_COLOR, BACKGROUND_COLOR);
	DrawString(font_16, SNIFFER_USB_PD_LEFT, SNIFFER_USB_PD_TOP, str_usbpd, NORMAL_COLOR, BACKGROUND_COLOR);
	DrawString(font_16, SNIFFER_AUTO_DETECT_LEFT, SNIFFER_AUTO_DETECT_TOP, str_autodetect, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawString(font_16, SNIFFER_BACK_LEFT, SNIFFER_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawSnifferNormalQC20()
{
	DrawString(font_16, SNIFFER_QC20_LEFT, SNIFFER_QC20_TOP, str_qc20, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_QC20_LEFT - SNIFFER_DELTA, SNIFFER_QC20_TOP - SNIFFER_DELTA,
					SNIFFER_QC20_LEFT + GetStringWidth(font_16, str_qc20) + SNIFFER_DELTA, 
					SNIFFER_QC20_TOP + 16 + SNIFFER_DELTA,
					BACKGROUND_COLOR);
}

void DrawSnifferNormalQC30()
{
	DrawString(font_16, SNIFFER_QC30_LEFT, SNIFFER_QC30_TOP, str_qc30, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_QC30_LEFT - SNIFFER_DELTA, SNIFFER_QC30_TOP - SNIFFER_DELTA,
					SNIFFER_QC30_LEFT + GetStringWidth(font_16, str_qc30) + SNIFFER_DELTA, 
					SNIFFER_QC30_TOP + 16 + SNIFFER_DELTA,
					BACKGROUND_COLOR);
}

void DrawSnifferNormalPD()
{
	DrawString(font_16, SNIFFER_USB_PD_LEFT, SNIFFER_USB_PD_TOP, str_usbpd, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_USB_PD_LEFT - SNIFFER_DELTA, SNIFFER_USB_PD_TOP - SNIFFER_DELTA,
					SNIFFER_USB_PD_LEFT + GetStringWidth(font_16, str_usbpd) + SNIFFER_DELTA, 
					SNIFFER_USB_PD_TOP + 16 + SNIFFER_DELTA,
					BACKGROUND_COLOR);
}

void DrawSnifferNormalAutoDetect()
{
	DrawString(font_16, SNIFFER_AUTO_DETECT_LEFT, SNIFFER_AUTO_DETECT_TOP, str_autodetect, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_AUTO_DETECT_LEFT - SNIFFER_DELTA, SNIFFER_AUTO_DETECT_TOP - SNIFFER_DELTA,
					SNIFFER_AUTO_DETECT_LEFT + GetStringWidth(font_16, str_autodetect) + SNIFFER_DELTA, 
					SNIFFER_AUTO_DETECT_TOP + 16 + SNIFFER_DELTA,
					BACKGROUND_COLOR);
}

void DrawSnifferNormalBack()
{
	DrawString(font_16, SNIFFER_BACK_LEFT, SNIFFER_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_BACK_LEFT - SNIFFER_DELTA, SNIFFER_BACK_TOP - SNIFFER_DELTA,
					SNIFFER_BACK_LEFT + GetStringWidth(font_16, str_back) + SNIFFER_DELTA, 
					SNIFFER_BACK_TOP + 16 + SNIFFER_DELTA,
					BACKGROUND_COLOR);
}

void DrawSnifferSelQC20()
{
	DrawString(font_16, SNIFFER_QC20_LEFT, SNIFFER_QC20_TOP, str_qc20, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_QC20_LEFT - SNIFFER_DELTA, SNIFFER_QC20_TOP - SNIFFER_DELTA,
					SNIFFER_QC20_LEFT + GetStringWidth(font_16, str_qc20) + SNIFFER_DELTA, 
					SNIFFER_QC20_TOP + 16 + SNIFFER_DELTA,
					SELECTED_COLOR);
}

void DrawSnifferSelQC30()
{
	DrawString(font_16, SNIFFER_QC30_LEFT, SNIFFER_QC30_TOP, str_qc30, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_QC30_LEFT - SNIFFER_DELTA, SNIFFER_QC30_TOP - SNIFFER_DELTA,
					SNIFFER_QC30_LEFT + GetStringWidth(font_16, str_qc30) + SNIFFER_DELTA, 
					SNIFFER_QC30_TOP + 16 + SNIFFER_DELTA,
					SELECTED_COLOR);
}

void DrawSnifferSelPD()
{
	DrawString(font_16, SNIFFER_USB_PD_LEFT, SNIFFER_USB_PD_TOP, str_usbpd, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_USB_PD_LEFT - SNIFFER_DELTA, SNIFFER_USB_PD_TOP - SNIFFER_DELTA,
					SNIFFER_USB_PD_LEFT + GetStringWidth(font_16, str_usbpd) + SNIFFER_DELTA, 
					SNIFFER_USB_PD_TOP + 16 + SNIFFER_DELTA,
					SELECTED_COLOR);
}

void DrawSnifferSelAutoDetect()
{
	DrawString(font_16, SNIFFER_AUTO_DETECT_LEFT, SNIFFER_AUTO_DETECT_TOP, str_autodetect, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_AUTO_DETECT_LEFT - SNIFFER_DELTA, SNIFFER_AUTO_DETECT_TOP - SNIFFER_DELTA,
					SNIFFER_AUTO_DETECT_LEFT + GetStringWidth(font_16, str_autodetect) + SNIFFER_DELTA, 
					SNIFFER_AUTO_DETECT_TOP + 16 + SNIFFER_DELTA,
					SELECTED_COLOR);
}

void DrawSnifferSelBack()
{
	DrawString(font_16, SNIFFER_BACK_LEFT, SNIFFER_BACK_TOP, str_back, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SNIFFER_BACK_LEFT - SNIFFER_DELTA, SNIFFER_BACK_TOP - SNIFFER_DELTA,
				SNIFFER_BACK_LEFT + GetStringWidth(font_16, str_back) + SNIFFER_DELTA, 
				SNIFFER_BACK_TOP + 16 + SNIFFER_DELTA,
				WHITE);
}

//qc2.0 menu
#define QC20_TITLE_LEFT           28
#define QC20_TITLE_TOP            5

#define QC20_ITEM_TOP             35
#define QC20_ITEM_DELTA           3
#define QC20_ITEM_5V_LEFT         5
#define QC20_ITEM_9V_LEFT         35
#define QC20_ITEM_12V_LEFT        62
#define QC20_ITEM_20V_LEFT        97

#define QC20_VOLTAGE_LEFT         3
#define QC20_VOLTAGE_UNIT_LEFT    108
#define QC20_VOLTAGE_TOP          60

#define QC20_CURRENT_LEFT         3
#define QC20_CURRENT_UNIT_LEFT    108
#define QC20_CURRENT_TOP          80

#define QC20_BACK_LEFT            80
#define QC20_BACK_TOP             108

void DrawQC20Menu()
{
	//title
	DrawString(font_20, QC20_TITLE_LEFT, QC20_TITLE_TOP, str_qc20, CYAN, BACKGROUND_COLOR);

	DrawString(font_16, QC20_ITEM_5V_LEFT, QC20_ITEM_TOP, str_5v, NORMAL_COLOR, BACKGROUND_COLOR);
	DrawString(font_16, QC20_ITEM_9V_LEFT, QC20_ITEM_TOP, str_9v, NORMAL_COLOR, BACKGROUND_COLOR);
	DrawString(font_16, QC20_ITEM_12V_LEFT, QC20_ITEM_TOP, str_12v, NORMAL_COLOR, BACKGROUND_COLOR);
	DrawString(font_16, QC20_ITEM_20V_LEFT, QC20_ITEM_TOP, str_20v, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawString(font_20, QC20_VOLTAGE_UNIT_LEFT, QC20_VOLTAGE_TOP, "V", CYAN, BACKGROUND_COLOR);
	DrawString(font_20, QC20_CURRENT_UNIT_LEFT, QC20_CURRENT_TOP, "A", CYAN, BACKGROUND_COLOR);
	
	//back
	DrawString(font_16, QC20_BACK_LEFT, QC20_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawQC20Voltage(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, QC20_VOLTAGE_LEFT, QC20_VOLTAGE_TOP, dec, frac, CYAN);
}

void DrawQC20Current(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, QC20_CURRENT_LEFT, QC20_CURRENT_TOP, dec, frac, CYAN);
}

void DrawQC20ItemNormal5V()
{
	DrawString(font_16, QC20_ITEM_5V_LEFT, QC20_ITEM_TOP, str_5v, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC20_ITEM_5V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_5V_LEFT + GetStringWidth(font_16, str_5v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					BACKGROUND_COLOR);
}

void DrawQC20ItemNormal9V()
{
	DrawString(font_16, QC20_ITEM_9V_LEFT, QC20_ITEM_TOP, str_9v, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC20_ITEM_9V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_9V_LEFT + GetStringWidth(font_16, str_9v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					BACKGROUND_COLOR);
}

void DrawQC20ItemNormal12V()
{
	DrawString(font_16, QC20_ITEM_12V_LEFT, QC20_ITEM_TOP, str_12v, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC20_ITEM_12V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_12V_LEFT + GetStringWidth(font_16, str_12v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					BACKGROUND_COLOR);
}

void DrawQC20ItemNormal20V()
{
	DrawString(font_16, QC20_ITEM_20V_LEFT, QC20_ITEM_TOP, str_20v, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC20_ITEM_20V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_20V_LEFT + GetStringWidth(font_16, str_20v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					BACKGROUND_COLOR);
}

void DrawQC20NormalBack()
{
	DrawString(font_16, QC20_BACK_LEFT, QC20_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC20_BACK_LEFT - QC20_ITEM_DELTA, QC20_BACK_TOP - QC20_ITEM_DELTA,
					QC20_BACK_LEFT + GetStringWidth(font_16, str_back) + QC20_ITEM_DELTA, QC20_BACK_TOP + 16 + QC20_ITEM_DELTA,
					BACKGROUND_COLOR);
}

void DrawQC20ItemSel5V()
{
	DrawString(font_16, QC20_ITEM_5V_LEFT, QC20_ITEM_TOP, str_5v, NORMAL_COLOR, BACKGROUND_COLOR);
	
	DrawRectangle(QC20_ITEM_5V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_5V_LEFT + GetStringWidth(font_16, str_5v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					WHITE);
}

void DrawQC20ItemSel9V()
{

	DrawString(font_16, QC20_ITEM_9V_LEFT, QC20_ITEM_TOP, str_9v, NORMAL_COLOR, BACKGROUND_COLOR);
	
	DrawRectangle(QC20_ITEM_9V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_9V_LEFT + GetStringWidth(font_16, str_9v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					WHITE);
}

void DrawQC20ItemSel12V()
{
	DrawString(font_16, QC20_ITEM_12V_LEFT, QC20_ITEM_TOP, str_12v, NORMAL_COLOR, BACKGROUND_COLOR);
	
	DrawRectangle(QC20_ITEM_12V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_12V_LEFT + GetStringWidth(font_16, str_12v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					WHITE);
}

void DrawQC20ItemSel20V()
{
	DrawString(font_16, QC20_ITEM_20V_LEFT, QC20_ITEM_TOP, str_20v, NORMAL_COLOR, BACKGROUND_COLOR);
	
	DrawRectangle(QC20_ITEM_20V_LEFT - QC20_ITEM_DELTA, QC20_ITEM_TOP - QC20_ITEM_DELTA,
					QC20_ITEM_20V_LEFT + GetStringWidth(font_16, str_20v) + QC20_ITEM_DELTA, QC20_ITEM_TOP + 16 + QC20_ITEM_DELTA,
					WHITE);
}

void DrawQC20SelBack()
{
	DrawString(font_16, QC20_BACK_LEFT, QC20_BACK_TOP, str_back, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC20_BACK_LEFT - QC20_ITEM_DELTA, QC20_BACK_TOP - QC20_ITEM_DELTA,
					QC20_BACK_LEFT + GetStringWidth(font_16, str_back) + QC20_ITEM_DELTA, QC20_BACK_TOP + 16 + QC20_ITEM_DELTA,
					WHITE);
}


void DrawQC20ItemPress5V()
{
	DrawString(font_16, QC20_ITEM_5V_LEFT, QC20_ITEM_TOP, str_5v, PRESSED_COLOR, BACKGROUND_COLOR);
}

void DrawQC20ItemPress9V()
{
	DrawString(font_16, QC20_ITEM_9V_LEFT, QC20_ITEM_TOP, str_9v, PRESSED_COLOR, BACKGROUND_COLOR);
}

void DrawQC20ItemPress12V()
{
	DrawString(font_16, QC20_ITEM_12V_LEFT, QC20_ITEM_TOP, str_12v, PRESSED_COLOR, BACKGROUND_COLOR);
}

void DrawQC20ItemPress20V()
{
	DrawString(font_16, QC20_ITEM_20V_LEFT, QC20_ITEM_TOP, str_20v, PRESSED_COLOR, BACKGROUND_COLOR);
}

//qc3.0 menu
#define QC30_TITLE_LEFT           28
#define QC30_TITLE_TOP            5

#define QC30_ITEM_DELTA_X         6
#define QC30_ITEM_DELTA_Y         3
#define QC30_ITEM_DELTA           3

#define QC30_CHANGE_TOP           35
#define QC30_DECREASE_LEFT        20
#define QC30_INCREASE_LEFT        100

#define QC30_VOLTAGE_LEFT         3
#define QC30_VOLTAGE_UNIT_LEFT    108
#define QC30_VOLTAGE_TOP          60

#define QC30_CURRENT_LEFT         3
#define QC30_CURRENT_UNIT_LEFT    108
#define QC30_CURRENT_TOP          80

#define QC30_BACK_LEFT            80
#define QC30_BACK_TOP             108

void DrawQC30Menu()
{
	//title
	DrawString(font_20, QC30_TITLE_LEFT, QC30_TITLE_TOP, str_qc30, CYAN, BACKGROUND_COLOR);

	DrawString(font_16, QC30_DECREASE_LEFT, QC30_CHANGE_TOP, "-", NORMAL_COLOR, BACKGROUND_COLOR);
	DrawString(font_16, QC30_INCREASE_LEFT, QC30_CHANGE_TOP, "+", NORMAL_COLOR, BACKGROUND_COLOR);

	DrawString(font_20, QC30_VOLTAGE_UNIT_LEFT, QC30_VOLTAGE_TOP, "V", CYAN, BACKGROUND_COLOR);
	DrawString(font_20, QC30_CURRENT_UNIT_LEFT, QC30_CURRENT_TOP, "A", CYAN, BACKGROUND_COLOR);
	
	//back
	DrawString(font_16, QC30_BACK_LEFT, QC30_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawQC30Voltage(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, QC30_VOLTAGE_LEFT, QC30_VOLTAGE_TOP, dec, frac, CYAN);
}

void DrawQC30Current(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, QC30_CURRENT_LEFT, QC30_CURRENT_TOP, dec, frac, CYAN);
}

void DrawQC30NormalDecrease()
{
	DrawString(font_16, QC30_DECREASE_LEFT, QC30_CHANGE_TOP, "-", NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC30_DECREASE_LEFT - QC30_ITEM_DELTA_X, QC30_CHANGE_TOP - QC30_ITEM_DELTA_Y,
					QC30_DECREASE_LEFT + GetStringWidth(font_16, "-") + QC30_ITEM_DELTA_X, QC30_CHANGE_TOP + 16 + QC30_ITEM_DELTA_Y,
					BACKGROUND_COLOR);
}

void DrawQC30NormalIncrease()
{
	DrawString(font_16, QC30_INCREASE_LEFT, QC30_CHANGE_TOP, "+", NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC30_INCREASE_LEFT - QC30_ITEM_DELTA_X, QC30_CHANGE_TOP - QC30_ITEM_DELTA_Y,
					QC30_INCREASE_LEFT + GetStringWidth(font_16, "+") + QC30_ITEM_DELTA_X, QC30_CHANGE_TOP + 16 + QC30_ITEM_DELTA_Y,
					BACKGROUND_COLOR);
}

void DrawQC30NormalBack()
{
	DrawString(font_16, QC30_BACK_LEFT, QC30_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC30_BACK_LEFT - QC30_ITEM_DELTA, QC30_BACK_TOP - QC30_ITEM_DELTA,
					QC30_BACK_LEFT + GetStringWidth(font_16, str_back) + QC30_ITEM_DELTA, QC30_BACK_TOP + 16 + QC30_ITEM_DELTA,
					BACKGROUND_COLOR);
}

void DrawQC30SelDecrease()
{
	DrawString(font_16, QC30_DECREASE_LEFT, QC30_CHANGE_TOP, "-", NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC30_DECREASE_LEFT - QC30_ITEM_DELTA_X, QC30_CHANGE_TOP - QC30_ITEM_DELTA_Y,
					QC30_DECREASE_LEFT + GetStringWidth(font_16, "-") + QC30_ITEM_DELTA_X, QC30_CHANGE_TOP + 16 + QC30_ITEM_DELTA_Y,
					WHITE);
}

void DrawQC30SelIncrease()
{
	DrawString(font_16, QC30_INCREASE_LEFT, QC30_CHANGE_TOP, "+", NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC30_INCREASE_LEFT - QC30_ITEM_DELTA_X, QC30_CHANGE_TOP - QC30_ITEM_DELTA_Y,
					QC30_INCREASE_LEFT + GetStringWidth(font_16, "+") + QC30_ITEM_DELTA_X, QC30_CHANGE_TOP + 16 + QC30_ITEM_DELTA_Y,
					WHITE);
}

void DrawQC30SelBack()
{
	DrawString(font_16, QC30_BACK_LEFT, QC30_BACK_TOP, str_back, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(QC30_BACK_LEFT - QC30_ITEM_DELTA, QC30_BACK_TOP - QC30_ITEM_DELTA,
					QC30_BACK_LEFT + GetStringWidth(font_16, str_back) + QC30_ITEM_DELTA, QC30_BACK_TOP + 16 + QC30_ITEM_DELTA,
					WHITE);
}

void DrawQC30PressDecrease()
{
	DrawString(font_16, QC30_DECREASE_LEFT, QC30_CHANGE_TOP, "-", PRESSED_COLOR, BACKGROUND_COLOR);
}

void DrawQC30PressIncrease()
{
	DrawString(font_16, QC30_INCREASE_LEFT, QC30_CHANGE_TOP, "+", PRESSED_COLOR, BACKGROUND_COLOR);
}

//usb pd list menu
#define PD_LIST_TITLE_LEFT        16
#define PD_LIST_TITLE_TOP         0

#define PD_LIST_TIP_LEFT          1
#define PD_LIST_TIP_TOP           60

#define PD_LIST_LEFT              5
#define PD_LIST_TOP               21

#define PD_LIST_INTERVAL          1

#define PD_LIST_DELTA             1

void DrawPDListMenu()
{
	//title
	DrawString(font_20, PD_LIST_TITLE_LEFT, PD_LIST_TITLE_TOP, str_usbpd, CYAN, BACKGROUND_COLOR);
}

void DrawPDListFailed()
{
	DrawString(font_16, PD_LIST_TIP_LEFT, PD_LIST_TIP_TOP, str_get_pd_failed, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawPDCapabilityList(const CapabilityStruct *caps, uint8 cnt)
{
	char buf[20];

	uint8 VDec, VFrac;
	uint8 ADec, AFrac;

	uint8 i;
	for (i = 0; i < cnt; i++)
	{
		VDec = caps[i].voltage / 1000;
		VFrac = caps[i].voltage / 10 % 100;

		ADec = caps[i].maxMa / 1000;
		AFrac = caps[i].maxMa / 10 % 100;

		if (VDec < 10)
		{
			sprintf(buf, "%d:  %d.%02dV %d.%02dA", i + 1, VDec, VFrac, ADec, AFrac);
		}
		else
		{
			sprintf(buf, "%d: %d.%02dV %d.%02dA", i + 1, VDec, VFrac, ADec, AFrac);
		}

		DrawString(font_16, PD_LIST_LEFT, PD_LIST_TOP + (PD_LIST_INTERVAL + 16) * i, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
	}
}

void DrawPDNormalCapability(uint8 index, const CapabilityStruct *caps)
{
	char buf[20];

	uint8 VDec, VFrac;
	uint8 ADec, AFrac;

	VDec = caps->voltage / 1000;
	VFrac = caps->voltage / 10 % 100;

	ADec = caps->maxMa / 1000;
	AFrac = caps->maxMa / 10 % 100;

	if (VDec < 10)
	{
		sprintf(buf, "%d:  %d.%02dV %d.%02dA", index + 1, VDec, VFrac, ADec, AFrac);
	}
	else
	{
		sprintf(buf, "%d: %d.%02dV %d.%02dA", index + 1, VDec, VFrac, ADec, AFrac);
	}

	uint16 y = PD_LIST_TOP + (PD_LIST_INTERVAL + 16) * index;
	DrawString(font_16, PD_LIST_LEFT, y, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(PD_LIST_LEFT - PD_LIST_DELTA, y - PD_LIST_DELTA,
					PD_LIST_LEFT + GetStringWidth(font_16, (uint8 *)buf) + PD_LIST_DELTA,
					y + 16 + PD_LIST_DELTA,
					BACKGROUND_COLOR);
}

void DrawPDSelCapability(uint8 index, const CapabilityStruct *caps)
{
	char buf[20];

	uint8 VDec, VFrac;
	uint8 ADec, AFrac;

	VDec = caps->voltage / 1000;
	VFrac = caps->voltage / 10 % 100;

	ADec = caps->maxMa / 1000;
	AFrac = caps->maxMa / 10 % 100;

	if (VDec < 10)
	{
		sprintf(buf, "%d:  %d.%02dV %d.%02dA", index + 1, VDec, VFrac, ADec, AFrac);
	}
	else
	{
		sprintf(buf, "%d: %d.%02dV %d.%02dA", index + 1, VDec, VFrac, ADec, AFrac);
	}

	uint16 y = PD_LIST_TOP + (PD_LIST_INTERVAL + 16) * index;
	DrawString(font_16, PD_LIST_LEFT, y, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(PD_LIST_LEFT - PD_LIST_DELTA, y - PD_LIST_DELTA,
					PD_LIST_LEFT + GetStringWidth(font_16, (uint8 *)buf) + PD_LIST_DELTA,
					y + 16 + PD_LIST_DELTA,
					SELECTED_COLOR);
}

//pd menu
#define PD_TITLE_LEFT             16
#define PD_TITLE_TOP               5

#define PD_ITEM_LEFT               5
#define PD_ITEM_TOP               30

#define PD_ITEM_DELTA           3

#define PD_VOLTAGE_LEFT         3
#define PD_VOLTAGE_UNIT_LEFT    108
#define PD_VOLTAGE_TOP          60

#define PD_CURRENT_LEFT         3
#define PD_CURRENT_UNIT_LEFT    108
#define PD_CURRENT_TOP          80

#define PD_TIP_LEFT             8
#define PD_TIP_TOP              108

#define PD_BACK_LEFT            80
#define PD_BACK_TOP             108

void DrawPDMenu()
{
	//title
	DrawString(font_20, PD_TITLE_LEFT, PD_TITLE_TOP, str_usbpd, CYAN, BACKGROUND_COLOR);

	DrawString(font_20, PD_VOLTAGE_UNIT_LEFT, PD_VOLTAGE_TOP, "V", CYAN, BACKGROUND_COLOR);
	DrawString(font_20, PD_CURRENT_UNIT_LEFT, PD_CURRENT_TOP, "A", CYAN, BACKGROUND_COLOR);

	DrawString(font_16, PD_BACK_LEFT, PD_BACK_TOP, str_back, SELECTED_COLOR, BACKGROUND_COLOR);
	
	DrawRectangle(PD_BACK_LEFT - PD_ITEM_DELTA, PD_BACK_TOP - PD_ITEM_DELTA,
					PD_BACK_LEFT + GetStringWidth(font_16, str_back) + PD_ITEM_DELTA, PD_BACK_TOP + 16 + PD_ITEM_DELTA,
					WHITE);
}

void DrawPDItem(uint8 index, const CapabilityStruct *caps)
{	
	char buf[20];
	
	uint8 VDec, VFrac;
	uint8 ADec, AFrac;

	VDec = caps->voltage / 1000;
	VFrac = caps->voltage / 10 % 100;

	ADec = caps->maxMa / 1000;
	AFrac = caps->maxMa / 10 % 100;

	if (VDec < 10)
	{
		sprintf(buf, "%d:  %d.%02dV %d.%02dA", index + 1, VDec, VFrac, ADec, AFrac);
	}
	else
	{
		sprintf(buf, "%d: %d.%02dV %d.%02dA", index + 1, VDec, VFrac, ADec, AFrac);
	}
	
	DrawString(font_16, PD_ITEM_LEFT, PD_ITEM_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawPDVoltage(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, PD_VOLTAGE_LEFT, PD_VOLTAGE_TOP, dec, frac, CYAN);
}

void DrawPDCurrent(uint8 dec, uint16 frac)
{
	DrawVBusValue(font_20, PD_CURRENT_LEFT, PD_CURRENT_TOP, dec, frac, CYAN);
}

void DrawPDTipAccept()
{
	DrawString(font_16, PD_TIP_LEFT, PD_TIP_TOP, str_accept, CYAN, BACKGROUND_COLOR);
}

void DrawPDTipReject()
{
	DrawString(font_16, PD_TIP_LEFT, PD_TIP_TOP, str_reject, CYAN, BACKGROUND_COLOR);
}

void DrawPDTipPsRdy()
{
	DrawString(font_16, PD_TIP_LEFT, PD_TIP_TOP, str_ps_rdy, CYAN, BACKGROUND_COLOR);
}

void DrawPDTipFailed()
{
	DrawString(font_16, PD_TIP_LEFT, PD_TIP_TOP, str_failed, CYAN, BACKGROUND_COLOR);
}

void ClearPDTip()
{
	DrawString(font_16, PD_TIP_LEFT, PD_TIP_TOP, "      ", CYAN, BACKGROUND_COLOR);
}

//auto detect menu
#define AUTO_TITLE_LEFT            32
#define AUTO_TITLE_TOP             0

#define AUTO_DCP_LEFT              8
#define AUTO_DCP_TOP               25

#define AUTO_QC20_LEFT             8
#define AUTO_QC20_TOP              41
#define AUTO_QC20_9V_LEFT          (AUTO_QC20_LEFT + 44)
#define AUTO_QC20_12V_LEFT         (AUTO_QC20_LEFT + 64)
#define AUTO_QC20_20V_LEFT         (AUTO_QC20_LEFT + 92)

#define AUTO_QC30_LEFT             8
#define AUTO_QC30_TOP              57

#define AUTO_APPLE_21A_LEFT        8
#define AUTO_APPLE_21A_TOP         73

#define AUTO_PD_LEFT               8
#define AUTO_PD_TOP                89

#define AUTO_BACK_LEFT            80
#define AUTO_BACK_TOP             108
#define AUTO_DELTA                2

void DrawAutoDetectMenu()
{
	//title
	DrawString(font_20, AUTO_TITLE_LEFT, AUTO_TITLE_TOP, str_auto, CYAN, BACKGROUND_COLOR);

	//dcp
	DrawString(font_16, AUTO_DCP_LEFT, AUTO_DCP_TOP, str_dcp, WHITE, BACKGROUND_COLOR);

	//qc2.0
	DrawString(font_16, AUTO_QC20_LEFT, AUTO_QC20_TOP, str_qc20, WHITE, BACKGROUND_COLOR);
	DrawString(font_16, AUTO_QC20_9V_LEFT, AUTO_QC20_TOP, str_9v, WHITE, BACKGROUND_COLOR);
	DrawString(font_16, AUTO_QC20_12V_LEFT, AUTO_QC20_TOP, str_12v, WHITE, BACKGROUND_COLOR);
	DrawString(font_16, AUTO_QC20_20V_LEFT, AUTO_QC20_TOP, str_20v, WHITE, BACKGROUND_COLOR);

	//qc3.0
	DrawString(font_16, AUTO_QC30_LEFT, AUTO_QC30_TOP, str_qc30, WHITE, BACKGROUND_COLOR);

	//apple 2.1A
	DrawString(font_16, AUTO_APPLE_21A_LEFT, AUTO_APPLE_21A_TOP, str_apple_21a, WHITE, BACKGROUND_COLOR);

	//usb pd
	DrawString(font_16, AUTO_PD_LEFT, AUTO_PD_TOP, str_usbpd, WHITE, BACKGROUND_COLOR);

	//back
	DrawString(font_16, AUTO_BACK_LEFT, AUTO_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);
}

#define SUPPORT_COLOR     GREEN
#define UNSUPPORT_COLOR   RED

void DrawAutoDcpSupport()
{
	DrawString(font_16, AUTO_DCP_LEFT, AUTO_DCP_TOP, str_dcp, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoDcpUnsupport()
{
	DrawString(font_16, AUTO_DCP_LEFT, AUTO_DCP_TOP, str_dcp, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc20Support()
{
	DrawString(font_16, AUTO_QC20_LEFT, AUTO_QC20_TOP, str_qc20, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc209VSupport()
{
	DrawString(font_16, AUTO_QC20_9V_LEFT, AUTO_QC20_TOP, str_9v, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc2012VSupport()
{
	DrawString(font_16, AUTO_QC20_12V_LEFT, AUTO_QC20_TOP, str_12v, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc2020VSupport()
{
	DrawString(font_16, AUTO_QC20_20V_LEFT, AUTO_QC20_TOP, str_20v, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc20Unsupport()
{
	DrawString(font_16, AUTO_QC20_LEFT, AUTO_QC20_TOP, str_qc20, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc209VUnsupport()
{
	DrawString(font_16, AUTO_QC20_9V_LEFT, AUTO_QC20_TOP, str_9v, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc2012VUnsupport()
{
	DrawString(font_16, AUTO_QC20_12V_LEFT, AUTO_QC20_TOP, str_12v, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc2020VUnsupport()
{
	DrawString(font_16, AUTO_QC20_20V_LEFT, AUTO_QC20_TOP, str_20v, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc30Support()
{
	DrawString(font_16, AUTO_QC30_LEFT, AUTO_QC30_TOP, str_qc30, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoQc30Unsupport()
{
	DrawString(font_16, AUTO_QC30_LEFT, AUTO_QC30_TOP, str_qc30, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoApple21ASupport()
{
	DrawString(font_16, AUTO_APPLE_21A_LEFT, AUTO_APPLE_21A_TOP, str_apple_21a, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoApple21AUnsupport()
{
	DrawString(font_16, AUTO_APPLE_21A_LEFT, AUTO_APPLE_21A_TOP, str_apple_21a, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoPdSupport()
{
	DrawString(font_16, AUTO_PD_LEFT, AUTO_PD_TOP, str_usbpd, SUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoPdUnsupport()
{
	DrawString(font_16, AUTO_PD_LEFT, AUTO_PD_TOP, str_usbpd, UNSUPPORT_COLOR, BACKGROUND_COLOR);
}

void DrawAutoSelBack()
{
	DrawString(font_16, AUTO_BACK_LEFT, AUTO_BACK_TOP, str_back, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(AUTO_BACK_LEFT - AUTO_DELTA, AUTO_BACK_TOP - AUTO_DELTA,
				AUTO_BACK_LEFT + GetStringWidth(font_16, str_back) + AUTO_DELTA,
				AUTO_BACK_TOP + 16 + AUTO_DELTA, SELECTED_COLOR);
}

//setting menu
#define SETTING_TITLE_LEFT        8
#define SETTING_TITLE_TOP         10

#define SETTING_DELTA             2

#define SETTING_TIME_LEFT         3
#define SETTING_TIME_TOP          40

#define SETTING_SAMPLING_LEFT     67
#define SETTING_SAMPLING_TOP      40

#define SETTING_SCREEN_LEFT       3
#define SETTING_SCREEN_TOP        60

#define SETTING_CALIBRATION_LEFT  67
#define SETTING_CALIBRATION_TOP   60

#define SETTING_BLE_LEFT          3
#define SETTING_BLE_TOP           80

#define SETTING_VERSION_LEFT      67
#define SETTING_VERSION_TOP       80

#define SETTING_BACK_LEFT         67
#define SETTING_BACK_TOP          105

void DrawSettingMenu()
{
	//title
	DrawString(font_20, SETTING_TITLE_LEFT, SETTING_TITLE_TOP, str_setting, CYAN, BACKGROUND_COLOR);

	DrawString(font_16, SETTING_TIME_LEFT, SETTING_TIME_TOP, str_time, ORANGE, BACKGROUND_COLOR);

	DrawString(font_16, SETTING_SAMPLING_LEFT, SETTING_SAMPLING_TOP, str_sampling, ORANGE, BACKGROUND_COLOR);

	DrawString(font_16, SETTING_SCREEN_LEFT, SETTING_SCREEN_TOP, str_screen, ORANGE, BACKGROUND_COLOR);

	DrawString(font_16, SETTING_CALIBRATION_LEFT, SETTING_CALIBRATION_TOP, str_calibration, ORANGE, BACKGROUND_COLOR);

	DrawString(font_16, SETTING_BLE_LEFT, SETTING_BLE_TOP, str_ble, ORANGE, BACKGROUND_COLOR);

	DrawString(font_16, SETTING_VERSION_LEFT, SETTING_VERSION_TOP, str_version, ORANGE, BACKGROUND_COLOR);

	DrawString(font_16, SETTING_BACK_LEFT, SETTING_BACK_TOP, str_back, ORANGE, BACKGROUND_COLOR);
}

//normal
void DrawSettingTimeNormalItem()
{
	DrawString(font_16, SETTING_TIME_LEFT, SETTING_TIME_TOP, str_time, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_TIME_LEFT - SETTING_DELTA, SETTING_TIME_TOP - SETTING_DELTA,
					SETTING_TIME_LEFT + GetStringWidth(font_16, str_time) + SETTING_DELTA, SETTING_TIME_TOP + 16 + SETTING_DELTA,
					BACKGROUND_COLOR);
}

void DrawSettingSampleNormalItem()
{
	DrawString(font_16, SETTING_SAMPLING_LEFT, SETTING_SAMPLING_TOP, str_sampling, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_SAMPLING_LEFT - SETTING_DELTA, SETTING_SAMPLING_TOP - SETTING_DELTA,
					SETTING_SAMPLING_LEFT + GetStringWidth(font_16, str_sampling) + SETTING_DELTA, SETTING_SAMPLING_TOP + 16 + SETTING_DELTA,
					BACKGROUND_COLOR);
}

void DrawSettingScreenNormalItem()
{
	DrawString(font_16, SETTING_SCREEN_LEFT, SETTING_SCREEN_TOP, str_screen, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_SCREEN_LEFT - SETTING_DELTA, SETTING_SCREEN_TOP - SETTING_DELTA,
					SETTING_SCREEN_LEFT + GetStringWidth(font_16, str_screen) + SETTING_DELTA, SETTING_SCREEN_TOP + 16 + SETTING_DELTA,
					BACKGROUND_COLOR);
}

void DrawSettingCalibrationNormalItem()
{
	DrawString(font_16, SETTING_CALIBRATION_LEFT, SETTING_CALIBRATION_TOP, str_calibration, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_CALIBRATION_LEFT - SETTING_DELTA, SETTING_CALIBRATION_TOP - SETTING_DELTA,
					SETTING_CALIBRATION_LEFT + GetStringWidth(font_16, str_calibration) + SETTING_DELTA, SETTING_CALIBRATION_TOP + 16 + SETTING_DELTA,
					BACKGROUND_COLOR);
}

void DrawSettingBleNormalItem()
{
	DrawString(font_16, SETTING_BLE_LEFT, SETTING_BLE_TOP, str_ble, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_BLE_LEFT - SETTING_DELTA, SETTING_BLE_TOP - SETTING_DELTA,
					SETTING_BLE_LEFT + GetStringWidth(font_16, str_ble) + SETTING_DELTA, SETTING_BLE_TOP + 16 + SETTING_DELTA,
					BACKGROUND_COLOR);
}

void DrawSettingVersionNormalItem()
{
	DrawString(font_16, SETTING_VERSION_LEFT, SETTING_VERSION_TOP, str_version, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_VERSION_LEFT - SETTING_DELTA, SETTING_VERSION_TOP - SETTING_DELTA,
					SETTING_VERSION_LEFT + GetStringWidth(font_16, str_version) + SETTING_DELTA, SETTING_VERSION_TOP + 16 + SETTING_DELTA,
					BACKGROUND_COLOR);
}

void DrawSettingBackNormal()
{
	DrawString(font_16, SETTING_BACK_LEFT, SETTING_BACK_TOP, str_back, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_BACK_LEFT - SETTING_DELTA, SETTING_BACK_TOP - SETTING_DELTA,
					SETTING_BACK_LEFT + GetStringWidth(font_16, str_back) + SETTING_DELTA, SETTING_BACK_TOP + 16 + SETTING_DELTA,
					BACKGROUND_COLOR);

}

//selected
void DrawSettingTimeSelItem()
{
	DrawString(font_16, SETTING_TIME_LEFT, SETTING_TIME_TOP, str_time, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_TIME_LEFT - SETTING_DELTA, SETTING_TIME_TOP - SETTING_DELTA,
					SETTING_TIME_LEFT + GetStringWidth(font_16, str_time) + SETTING_DELTA, SETTING_TIME_TOP + 16 + SETTING_DELTA,
					SELECTED_COLOR);
}

void DrawSettingSampleSelItem()
{
	DrawString(font_16, SETTING_SAMPLING_LEFT, SETTING_SAMPLING_TOP, str_sampling, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_SAMPLING_LEFT - SETTING_DELTA, SETTING_SAMPLING_TOP - SETTING_DELTA,
					SETTING_SAMPLING_LEFT + GetStringWidth(font_16, str_sampling) + SETTING_DELTA, SETTING_SAMPLING_TOP + 16 + SETTING_DELTA,
					SELECTED_COLOR);
}

void DrawSettingScreenSelItem()
{
	DrawString(font_16, SETTING_SCREEN_LEFT, SETTING_SCREEN_TOP, str_screen, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_SCREEN_LEFT - SETTING_DELTA, SETTING_SCREEN_TOP - SETTING_DELTA,
					SETTING_SCREEN_LEFT + GetStringWidth(font_16, str_screen) + SETTING_DELTA, SETTING_SCREEN_TOP + 16 + SETTING_DELTA,
					SELECTED_COLOR);
}

void DrawSettingCalibrationSelItem()
{
	DrawString(font_16, SETTING_CALIBRATION_LEFT, SETTING_CALIBRATION_TOP, str_calibration, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_CALIBRATION_LEFT - SETTING_DELTA, SETTING_CALIBRATION_TOP - SETTING_DELTA,
					SETTING_CALIBRATION_LEFT + GetStringWidth(font_16, str_calibration) + SETTING_DELTA, SETTING_CALIBRATION_TOP + 16 + SETTING_DELTA,
					SELECTED_COLOR);
}

void DrawSettingBleSelItem()
{
	DrawString(font_16, SETTING_BLE_LEFT, SETTING_BLE_TOP, str_ble, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_BLE_LEFT - SETTING_DELTA, SETTING_BLE_TOP - SETTING_DELTA,
					SETTING_BLE_LEFT + GetStringWidth(font_16, str_ble) + SETTING_DELTA, SETTING_BLE_TOP + 16 + SETTING_DELTA,
					SELECTED_COLOR);
}

void DrawSettingVersionSelItem()
{
	DrawString(font_16, SETTING_VERSION_LEFT, SETTING_VERSION_TOP, str_version, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_VERSION_LEFT - SETTING_DELTA, SETTING_VERSION_TOP - SETTING_DELTA,
					SETTING_VERSION_LEFT + GetStringWidth(font_16, str_version) + SETTING_DELTA, SETTING_VERSION_TOP + 16 + SETTING_DELTA,
					SELECTED_COLOR);
}

void DrawSettingBackSel()
{
	DrawString(font_16, SETTING_BACK_LEFT, SETTING_BACK_TOP, str_back, SELECTED_COLOR, BACKGROUND_COLOR);

	DrawRectangle(SETTING_BACK_LEFT - SETTING_DELTA, SETTING_BACK_TOP - SETTING_DELTA,
					SETTING_BACK_LEFT + GetStringWidth(font_16, str_back) + SETTING_DELTA, SETTING_BACK_TOP + 16 + SETTING_DELTA,
					WHITE);
}

//time
#define TIME_TITLE_LEFT     32
#define TIME_TITLE_TOP      10

#define TIME_YEAR_TOP       40
#define TIME_YEAR_LEFT      32

#define TIME_MONTH_TOP      60
#define TIME_MONTH_LEFT     28
#define TIME_DAY_LEFT       68

#define TIME_TIME_TOP       80
#define TIME_HOUR_LEFT      8
#define TIME_MINUTE_LEFT    48
#define TIME_SECOND_LEFT    88

#define TIME_OK_LEFT        40
#define TIME_OK_TOP         110

#define TIME_CANCEL_LEFT    64

void DrawTimeMenu(const TimeStruct *tm)
{
	//title
	DrawString(font_20, TIME_TITLE_LEFT, TIME_TITLE_TOP, str_time, CYAN, BACKGROUND_COLOR);

	//date
	char buf[20];
	sprintf(buf, "%04d", tm->year);
	DrawString(font_20, TIME_YEAR_LEFT, TIME_YEAR_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);

	sprintf(buf, "%02d-%02d", tm->month, tm->day);
	DrawString(font_20, TIME_MONTH_LEFT, TIME_MONTH_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
	
	sprintf(buf, "%02d:%02d:%02d", tm->hour, tm->minute, tm->second);
	DrawString(font_20, TIME_HOUR_LEFT, TIME_TIME_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawTimeNormalOK();
	DrawTimeNormalCancel();
}

void DrawTimeNormalYear(uint16 year)
{
	char buf[5];
	sprintf(buf, "%04d", year);
	DrawString(font_20, TIME_YEAR_LEFT, TIME_YEAR_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeNormalMonth(uint8 month)
{
	char buf[3];
	sprintf(buf, "%02d", month);
	DrawString(font_20, TIME_MONTH_LEFT, TIME_MONTH_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeNormalDay(uint8 day)
{
	char buf[3];
	sprintf(buf, "%02d", day);
	DrawString(font_20, TIME_DAY_LEFT, TIME_MONTH_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeNormalHour(uint8 hour)
{
	char buf[3];
	sprintf(buf, "%02d", hour);
	DrawString(font_20, TIME_HOUR_LEFT, TIME_TIME_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeNormalMinute(uint8 minute)
{
	char buf[3];
	sprintf(buf, "%02d", minute);
	DrawString(font_20, TIME_MINUTE_LEFT, TIME_TIME_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeNormalSecond(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, TIME_SECOND_LEFT, TIME_TIME_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeNormalOK()
{
	DrawString(font_16, TIME_OK_LEFT, TIME_OK_TOP, str_ok, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeNormalCancel()
{
	DrawString(font_16, TIME_CANCEL_LEFT, TIME_OK_TOP, str_cancel, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelYear(uint16 year)
{
	char buf[5];
	sprintf(buf, "%04d", year);
	DrawString(font_20, TIME_YEAR_LEFT, TIME_YEAR_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelMonth(uint8 month)
{
	char buf[3];
	sprintf(buf, "%02d", month);
	DrawString(font_20, TIME_MONTH_LEFT, TIME_MONTH_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelDay(uint8 day)
{
	char buf[3];
	sprintf(buf, "%02d", day);
	DrawString(font_20, TIME_DAY_LEFT, TIME_MONTH_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelHour(uint8 hour)
{
	char buf[3];
	sprintf(buf, "%02d", hour);
	DrawString(font_20, TIME_HOUR_LEFT, TIME_TIME_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelMinute(uint8 minute)
{
	char buf[3];
	sprintf(buf, "%02d", minute);
	DrawString(font_20, TIME_MINUTE_LEFT, TIME_TIME_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelSecond(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, TIME_SECOND_LEFT, TIME_TIME_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawTimeEditYear(uint16 year)
{
	char buf[5];
	sprintf(buf, "%04d", year);
	DrawString(font_20, TIME_YEAR_LEFT, TIME_YEAR_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawTimeEditMonth(uint8 month)
{
	char buf[3];
	sprintf(buf, "%02d", month);
	DrawString(font_20, TIME_MONTH_LEFT, TIME_MONTH_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawTimeEditDay(uint8 day)
{
	char buf[3];
	sprintf(buf, "%02d", day);
	DrawString(font_20, TIME_DAY_LEFT, TIME_MONTH_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawTimeEditHour(uint8 hour)
{
	char buf[3];
	sprintf(buf, "%02d", hour);
	DrawString(font_20, TIME_HOUR_LEFT, TIME_TIME_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawTimeEditMinute(uint8 minute)
{
	char buf[3];
	sprintf(buf, "%02d", minute);
	DrawString(font_20, TIME_MINUTE_LEFT, TIME_TIME_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawTimeEditSecond(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, TIME_SECOND_LEFT, TIME_TIME_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelOK()
{
	DrawString(font_16, TIME_OK_LEFT, TIME_OK_TOP, str_ok, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawTimeSelCancel()
{
	DrawString(font_16, TIME_CANCEL_LEFT, TIME_OK_TOP, str_cancel, SELECTED_COLOR, BACKGROUND_COLOR);
}

//sample
#define SAMPLE_TITLE_LEFT     16
#define SAMPLE_TITLE_TOP      10

#define SAMPLE_ADC_LEFT       0
#define SAMPLE_ADC_VAL_LEFT   56
#define SAMPLE_ADC_UNIT_LEFT  88
#define SAMPLE_ADC_TOP        45

#define SAMPLE_PEAK_VALLEY_DURATION_LEFT  0
#define SAMPLE_PEAK_VALLEY_DURATION_VAL_LEFT   72
#define SAMPLE_PEAK_VALLEY_DURATION_UNIT_LEFT  112
#define SAMPLE_PEAK_VALLEY_DURATION_TOP  75

#define SAMPLE_OK_LEFT        40
#define SAMPLE_OK_TOP         110

#define SAMPLE_CANCEL_LEFT    64

void DrawSampleMenu()
{
	//title
	DrawString(font_20, SAMPLE_TITLE_LEFT, SAMPLE_TITLE_TOP, str_sampling, CYAN, BACKGROUND_COLOR);

	DrawString(font_20, SAMPLE_ADC_LEFT, SAMPLE_ADC_TOP, str_adc, CYAN, BACKGROUND_COLOR);
	DrawString(font_20, SAMPLE_ADC_UNIT_LEFT, SAMPLE_ADC_TOP, str_fps, CYAN, BACKGROUND_COLOR);

	DrawString(font_20, SAMPLE_PEAK_VALLEY_DURATION_LEFT, SAMPLE_PEAK_VALLEY_DURATION_TOP, str_duration, CYAN, BACKGROUND_COLOR);
	DrawString(font_20, SAMPLE_PEAK_VALLEY_DURATION_UNIT_LEFT, SAMPLE_PEAK_VALLEY_DURATION_TOP, "S", CYAN, BACKGROUND_COLOR);

	DrawSampleNormalOK();
	DrawSampleNormalCancel();
}

void DrawSampleNormalADC(uint8 fps)
{
	char buf[3];
	sprintf(buf, "%02d", fps);
	DrawString(font_20, SAMPLE_ADC_VAL_LEFT, SAMPLE_ADC_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawSampleNormalDuration(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, SAMPLE_PEAK_VALLEY_DURATION_VAL_LEFT, SAMPLE_PEAK_VALLEY_DURATION_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawSampleNormalOK()
{
	DrawString(font_16, SAMPLE_OK_LEFT, SAMPLE_OK_TOP, str_ok, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawSampleNormalCancel()
{
	DrawString(font_16, SAMPLE_CANCEL_LEFT, SAMPLE_OK_TOP, str_cancel, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawSampleSelADC(uint8 fps)
{
	char buf[3];
	sprintf(buf, "%02d", fps);
	DrawString(font_20, SAMPLE_ADC_VAL_LEFT, SAMPLE_ADC_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawSampleSelDuration(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, SAMPLE_PEAK_VALLEY_DURATION_VAL_LEFT, SAMPLE_PEAK_VALLEY_DURATION_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawSampleSelOK()
{
	DrawString(font_16, SAMPLE_OK_LEFT, SAMPLE_OK_TOP, str_ok, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawSampleSelCancel()
{
	DrawString(font_16, SAMPLE_CANCEL_LEFT, SAMPLE_OK_TOP, str_cancel, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawSampleEditADC(uint8 fps)
{
	char buf[3];
	sprintf(buf, "%02d", fps);
	DrawString(font_20, SAMPLE_ADC_VAL_LEFT, SAMPLE_ADC_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawSampleEditDuration(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, SAMPLE_PEAK_VALLEY_DURATION_VAL_LEFT, SAMPLE_PEAK_VALLEY_DURATION_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}


//screen
#define SCREEN_TITLE_LEFT     16
#define SCREEN_TITLE_TOP      10

#define SCREEN_LOCK_LEFT      0
#define SCREEN_LOCK_VAL_LEFT  72
#define SCREEN_LOCK_UNIT_LEFT 112
#define SCREEN_LOCK_TOP       45

#define SCREEN_ANGLE_LEFT     0
#define SCREEN_ANGLE_VAL_LEFT 64
#define SCREEN_ANGLE_UNIT_LEFT 112
#define SCREEN_ANGLE_TOP      75

#define SCREEN_OK_LEFT        40
#define SCREEN_OK_TOP         110

#define SCREEN_CANCEL_LEFT    64

void DrawScreenMenu()
{
	//title
	DrawString(font_20, SCREEN_TITLE_LEFT, SCREEN_TITLE_TOP, str_screen, CYAN, BACKGROUND_COLOR);

	DrawString(font_20, SCREEN_LOCK_LEFT, SCREEN_LOCK_TOP, str_lock, CYAN, BACKGROUND_COLOR);
	DrawString(font_20, SCREEN_LOCK_UNIT_LEFT, SCREEN_LOCK_TOP, "S", CYAN, BACKGROUND_COLOR);

	DrawString(font_20, SCREEN_ANGLE_LEFT, SCREEN_ANGLE_TOP, str_angle, CYAN, BACKGROUND_COLOR);
	DrawString(font_20, SCREEN_ANGLE_UNIT_LEFT, SCREEN_ANGLE_TOP, "D", CYAN, BACKGROUND_COLOR);

	DrawScreenNormalOK();
	DrawScreenNormalCancel();
}

void DrawScreenNormalLock(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, SCREEN_LOCK_VAL_LEFT, SCREEN_LOCK_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawScreenNormalAngle(uint16 angle)
{
	char buf[4];
	sprintf(buf, "%3d", angle);
	DrawString(font_20, SCREEN_ANGLE_VAL_LEFT, SCREEN_ANGLE_TOP, (uint8 *)buf, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawScreenNormalOK()
{
	DrawString(font_16, SCREEN_OK_LEFT, SCREEN_OK_TOP, str_ok, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawScreenNormalCancel()
{
	DrawString(font_16, SCREEN_CANCEL_LEFT, SCREEN_OK_TOP, str_cancel, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawScreenSelLock(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, SCREEN_LOCK_VAL_LEFT, SCREEN_LOCK_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawScreenSelAngle(uint16 angle)
{
	char buf[4];
	sprintf(buf, "%3d", angle);
	DrawString(font_20, SCREEN_ANGLE_VAL_LEFT, SCREEN_ANGLE_TOP, (uint8 *)buf, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawScreenSelOK()
{
	DrawString(font_16, SCREEN_OK_LEFT, SCREEN_OK_TOP, str_ok, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawScreenSelCancel()
{
	DrawString(font_16, SCREEN_CANCEL_LEFT, SCREEN_OK_TOP, str_cancel, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawScreenEditLock(uint8 second)
{
	char buf[3];
	sprintf(buf, "%02d", second);
	DrawString(font_20, SCREEN_LOCK_VAL_LEFT, SCREEN_LOCK_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

void DrawScreenEditAngle(uint16 angle)
{
	char buf[4];
	sprintf(buf, "%3d", angle);
	DrawString(font_20, SCREEN_ANGLE_VAL_LEFT, SCREEN_ANGLE_TOP, (uint8 *)buf, EDIT_COLOR, BACKGROUND_COLOR);
}

//ble menu
#define BLE_TITLE_LEFT         40

#define BLE_TITLE_TOP          10

#define BLE_NAME_LEFT          0
#define BLE_NAME_TOP           40
#define BLE_NAME_VAL_LEFT      0
#define BLE_NAME_VAL_TOP       60

#define BLE_SWITCH_LEFT        0
#define BLE_SWITCH_TOP         85
#define BLE_SWITCH_VAL_LEFT    40
#define BLE_SWITCH_VAL_TOP     86

#define BLE_OK_LEFT            40
#define BLE_OK_TOP             110

#define BLE_CANCEL_LEFT        64

void DrawBleMenu()
{
	//title
	DrawString(font_20, BLE_TITLE_LEFT, BLE_TITLE_TOP, str_ble, CYAN, BACKGROUND_COLOR);

	DrawString(font_20, BLE_NAME_LEFT, BLE_NAME_TOP, str_name, CYAN, BACKGROUND_COLOR);
	DrawString(font_20, BLE_SWITCH_LEFT, BLE_SWITCH_TOP, str_switch, CYAN, BACKGROUND_COLOR);
	
	DrawBleNormalOK();
	DrawBleNormalCancel();
}

void DrawBleNormalName(const uint8 *name)
{
	DrawString(font_16, BLE_NAME_VAL_LEFT, BLE_NAME_VAL_TOP, name, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawBleNormalSwitch(bool on)
{
	const uint8 *p;
	if (on)
	{
		p = str_on;
	}
	else
	{
		p = str_off;
	}

	DrawString(font_16, BLE_SWITCH_VAL_LEFT, BLE_SWITCH_VAL_TOP, p, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawBleNormalOK()
{
	DrawString(font_16, BLE_OK_LEFT, BLE_OK_TOP, str_ok, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawBleNormalCancel()
{
	DrawString(font_16, BLE_CANCEL_LEFT, BLE_OK_TOP, str_cancel, NORMAL_COLOR, BACKGROUND_COLOR);
}

void DrawBleSelSwitch(bool on)
{
	const uint8 *p;
	if (on)
	{
		p = str_on;
	}
	else
	{
		p = str_off;
	}

	DrawString(font_16, BLE_SWITCH_VAL_LEFT, BLE_SWITCH_VAL_TOP, p, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawBleSelOK()
{
	DrawString(font_16, BLE_OK_LEFT, BLE_OK_TOP, str_ok, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawBleSelCancel()
{
	DrawString(font_16, BLE_CANCEL_LEFT, BLE_OK_TOP, str_cancel, SELECTED_COLOR, BACKGROUND_COLOR);
}

void DrawBleEditSwitch(bool on)
{
	const uint8 *p;
	if (on)
	{
		p = str_on;
	}
	else
	{
		p = str_off;
	}

	DrawString(font_16, BLE_SWITCH_VAL_LEFT, BLE_SWITCH_VAL_TOP, p, EDIT_COLOR, BACKGROUND_COLOR);
}

//version menu
#define VERSION_TITLE_LEFT      8
#define VERSION_TITLE_TOP       10

#define FIRMWARE_LEFT           0
#define FIRMWARE_TOP            40
#define FIRMWARE_VAL_LEFT       56
#define FIRMWARE_VAL_TOP        44

#define WEB_LEFT                0
#define WEB_TOP                 70
#define WEB_VAL_LEFT            0
#define WEB_VAL_TOP             90

void DrawVersionMenu()
{
	//title
	DrawString(font_20, VERSION_TITLE_LEFT, VERSION_TITLE_TOP, str_version, CYAN, BACKGROUND_COLOR);

	DrawString(font_20, FIRMWARE_LEFT, FIRMWARE_TOP, str_ver, CYAN, BACKGROUND_COLOR);
	DrawString(font_16, FIRMWARE_VAL_LEFT, FIRMWARE_VAL_TOP, FIRMWARE_VER, NORMAL_COLOR, BACKGROUND_COLOR);

	DrawString(font_20, WEB_LEFT, WEB_TOP, str_web, CYAN, BACKGROUND_COLOR);
	DrawString(font_16, WEB_VAL_LEFT, WEB_VAL_TOP, WEB_SITE, NORMAL_COLOR, BACKGROUND_COLOR);
}

//ble com mode
#define BLE_MODE_TITLE_LEFT     0
#define BLE_MODE_TITLE_TOP      50

void DrawBleComMenu()
{
	//title
	DrawString(font_20, BLE_MODE_TITLE_LEFT, BLE_MODE_TITLE_TOP, str_ble_mode, CYAN, BACKGROUND_COLOR);
}

//message
#define MESSAGE_YES_LEFT      50
#define MESSAGE_BOX_TOP       105

#define MESSAGE_NO_LEFT       100

#define MESSAGE_DELTA         3

void DrawMessageMenu(uint16 x, uint16 y, const uint8 *msg)
{
	//msg
	DrawString(font_16, x, y, msg, YELLOW, BACKGROUND_COLOR);
	
	DrawString(font_16, MESSAGE_YES_LEFT, MESSAGE_BOX_TOP, str_yes, WHITE, BACKGROUND_COLOR);
	DrawString(font_16, MESSAGE_NO_LEFT, MESSAGE_BOX_TOP, str_no, WHITE, BACKGROUND_COLOR);
}

void DrawMessageNormalYes()
{
	DrawRectangle(MESSAGE_YES_LEFT - MESSAGE_DELTA, MESSAGE_BOX_TOP - MESSAGE_DELTA,
					MESSAGE_YES_LEFT + GetStringWidth(font_16, str_yes) + MESSAGE_DELTA, MESSAGE_BOX_TOP + 16 + MESSAGE_DELTA,
					BACKGROUND_COLOR);
}

void DrawMessageNormalNo()
{
	DrawRectangle(MESSAGE_NO_LEFT - MESSAGE_DELTA, MESSAGE_BOX_TOP - MESSAGE_DELTA,
					MESSAGE_NO_LEFT + GetStringWidth(font_16, str_no) + MESSAGE_DELTA, MESSAGE_BOX_TOP + 16 + MESSAGE_DELTA,
					BACKGROUND_COLOR);
}

void DrawMessageSelYes()
{
	DrawRectangle(MESSAGE_YES_LEFT - MESSAGE_DELTA, MESSAGE_BOX_TOP - MESSAGE_DELTA,
					MESSAGE_YES_LEFT + GetStringWidth(font_16, str_yes) + MESSAGE_DELTA, MESSAGE_BOX_TOP + 16 + MESSAGE_DELTA,
					WHITE);
}

void DrawMessageSelNo()
{
	DrawRectangle(MESSAGE_NO_LEFT - MESSAGE_DELTA, MESSAGE_BOX_TOP - MESSAGE_DELTA,
					MESSAGE_NO_LEFT + GetStringWidth(font_16, str_no) + MESSAGE_DELTA, MESSAGE_BOX_TOP + 16 + MESSAGE_DELTA,
					WHITE);
}

