
#ifndef _DRAW_H_
#define _DRAW_H_

#include "RTC.h"
#include "UsbPd.h"
#include "menuOp.h"
#include "Global.h"

//menu color
#define BACKGROUND_COLOR       BLACK

#define CAPTION_COLOR          CYAN

#define NORMAL_COLOR           ORANGE
#define SELECTED_COLOR         WHITE
#define EDIT_COLOR             MAGENTA
#define PRESSED_COLOR          WHITE


//main menu
extern void DrawMainMenu();
extern void DrawMainVoltage(uint8 dec, uint16 frac);
extern void DrawMainVoltageDelta(uint8 dec, uint16 frac);

extern void DrawMainCurrent(uint8 dec, uint16 frac);
extern void DrawMainCurrentDelta(uint8 dec, uint16 frac);

extern void DrawMainPower(uint8 dec, uint16 frac);
extern void DrawMainPowerDelta(uint8 dec, uint16 frac);

extern void DrawMainDateTime(const TimeStruct *tm);
extern void DrawMainDateTimeDelta(const TimeStruct *time);

//detail menu
extern void DrawDetailMenu();
extern void DrawDetailDp(uint8 dec, uint16 frac);
extern void DrawDetailDm(uint8 dec, uint16 frac);
extern void DrawDetailSniffer(SnifferStatus sniffer, uint8 voltage);

extern void DrawDetailVoltage(uint8 dec, uint16 frac);
extern void DrawDetailVoltageDelta(uint8 dec, uint16 frac);

extern void DrawDetailCurrent(uint8 dec, uint16 frac);
extern void DrawDetailCurrentDelta(uint8 dec, uint16 frac);

extern void DrawDetailPower(uint8 dec, uint16 frac);
extern void DrawDetailPowerDelta(uint8 dec, uint16 frac);

extern void DrawDetailWh(uint16 dec, uint16 frac, uint16 fc);
extern void DrawDetailWhDelta(uint16 dec, uint16 frac, uint16 fc);

extern void DrawDetailAh(uint16 dec, uint16 frac, uint16 fc);
extern void DrawDetailAhDelta(uint16 dec, uint16 frac, uint16 fc);

//sniffer menu
extern void DrawSnifferMenu();

extern void DrawSnifferNormalQC20();
extern void DrawSnifferNormalQC30();
extern void DrawSnifferNormalPD();
extern void DrawSnifferNormalAutoDetect();

extern void DrawSnifferNormalBack();

extern void DrawSnifferSelQC20();
extern void DrawSnifferSelQC30();
extern void DrawSnifferSelPD();
extern void DrawSnifferSelAutoDetect();

extern void DrawSnifferSelBack();

//qc2.0 menu
extern void DrawQC20Menu();
extern void DrawQC20Voltage(uint8 dec, uint16 frac);
extern void DrawQC20VoltageDelta(uint8 dec, uint16 frac);

extern void DrawQC20Current(uint8 dec, uint16 frac);
extern void DrawQC20CurrentDelta(uint8 dec, uint16 frac);

extern void DrawQC20ItemNormal5V();
extern void DrawQC20ItemNormal9V();
extern void DrawQC20ItemNormal12V();
extern void DrawQC20ItemNormal20V();
extern void DrawQC20NormalBack();

extern void DrawQC20ItemSel5V();
extern void DrawQC20ItemSel9V();
extern void DrawQC20ItemSel12V();
extern void DrawQC20ItemSel20V();
extern void DrawQC20SelBack();

extern void DrawQC20ItemPress5V();
extern void DrawQC20ItemPress9V();
extern void DrawQC20ItemPress12V();
extern void DrawQC20ItemPress20V();

//qc3.0 menu
extern void DrawQC30Menu();
extern void DrawQC30Voltage(uint8 dec, uint16 frac);
extern void DrawQC30VoltageDelta(uint8 dec, uint16 frac);

extern void DrawQC30Current(uint8 dec, uint16 frac);
extern void DrawQC30CurrentDelta(uint8 dec, uint16 frac);

extern void DrawQC30NormalDecrease();
extern void DrawQC30NormalIncrease();
extern void DrawQC30NormalBack();

extern void DrawQC30SelDecrease();
extern void DrawQC30SelIncrease();
extern void DrawQC30SelBack();

extern void DrawQC30PressDecrease();
extern void DrawQC30PressIncrease();

//pd list menu
extern void DrawPDListMenu();
extern void DrawPDListFailed();
extern void DrawPDCapabilityList(const CapabilityStruct *caps, uint8 cnt);

extern void DrawPDNormalCapability(uint8 index, const CapabilityStruct *caps);

extern void DrawPDSelCapability(uint8 index, const CapabilityStruct *caps);

//pd menu
extern void DrawPDMenu();

extern void DrawPDItem(uint8 index, const CapabilityStruct *caps);
extern void DrawPDVoltage(uint8 dec, uint16 frac);
extern void DrawPDVoltageDelta(uint8 dec, uint16 frac);

extern void DrawPDCurrent(uint8 dec, uint16 frac);
extern void DrawPDCurrentDelta(uint8 dec, uint16 frac);

extern void DrawPDTipAccept();
extern void DrawPDTipReject();
extern void DrawPDTipPsRdy();
extern void DrawPDTipFailed();
extern void ClearPDTip();

//auto detect menu
extern void DrawAutoDetectMenu();

extern void DrawAutoDcpSupport();
extern void DrawAutoDcpUnsupport();

extern void DrawAutoQc20Support();
extern void DrawAutoQc209VSupport();
extern void DrawAutoQc2012VSupport();
extern void DrawAutoQc2020VSupport();

extern void DrawAutoQc20Unsupport();
extern void DrawAutoQc209VUnsupport();
extern void DrawAutoQc2012VUnsupport();
extern void DrawAutoQc2020VUnsupport();

extern void DrawAutoQc30Support();
extern void DrawAutoQc30Unsupport();

extern void DrawAutoApple21ASupport();
extern void DrawAutoApple21AUnsupport();

extern void DrawAutoPdSupport();
extern void DrawAutoPdUnsupport();

extern void DrawAutoSelBack();

//setting menu
extern void DrawSettingMenu();

extern void DrawSettingTimeNormalItem();
extern void DrawSettingSampleNormalItem();
extern void DrawSettingScreenNormalItem();
extern void DrawSettingCalibrationNormalItem();
extern void DrawSettingBleNormalItem();
extern void DrawSettingVersionNormalItem();
extern void DrawSettingBackNormal();

extern void DrawSettingTimeSelItem();
extern void DrawSettingSampleSelItem();
extern void DrawSettingScreenSelItem();
extern void DrawSettingCalibrationSelItem();
extern void DrawSettingBleSelItem();
extern void DrawSettingVersionSelItem();
extern void DrawSettingBackSel();

//time menu
extern void DrawTimeMenu(const TimeStruct *tm);

extern void DrawTimeNormalYear(uint16 year);
extern void DrawTimeNormalMonth(uint8 month);
extern void DrawTimeNormalDay(uint8 day);
extern void DrawTimeNormalHour(uint8 hour);
extern void DrawTimeNormalMinute(uint8 minute);
extern void DrawTimeNormalSecond(uint8 second);

extern void DrawTimeNormalOK();
extern void DrawTimeNormalCancel();

extern void DrawTimeSelYear(uint16 year);
extern void DrawTimeSelMonth(uint8 month);
extern void DrawTimeSelDay(uint8 day);
extern void DrawTimeSelHour(uint8 hour);
extern void DrawTimeSelMinute(uint8 minute);
extern void DrawTimeSelSecond(uint8 second);

extern void DrawTimeEditYear(uint16 year);
extern void DrawTimeEditMonth(uint8 month);
extern void DrawTimeEditDay(uint8 day);
extern void DrawTimeEditHour(uint8 hour);
extern void DrawTimeEditMinute(uint8 minute);
extern void DrawTimeEditSecond(uint8 second);

extern void DrawTimeSelOK();
extern void DrawTimeSelCancel();

//sample menu
extern void DrawSampleMenu();

extern void DrawSampleNormalADC(uint8 fps);
extern void DrawSampleNormalDuration(uint8 second);
extern void DrawSampleNormalOK();
extern void DrawSampleNormalCancel();

extern void DrawSampleSelADC(uint8 fps);
extern void DrawSampleSelDuration(uint8 second);
extern void DrawSampleSelOK();
extern void DrawSampleSelCancel();

extern void DrawSampleEditADC(uint8 fps);
extern void DrawSampleEditDuration(uint8 second);


//screen
extern void DrawScreenMenu();

extern void DrawScreenNormalLock(uint8 second);
extern void DrawScreenNormalAngle(uint16 angle);

extern void DrawScreenNormalOK();
extern void DrawScreenNormalCancel();

extern void DrawScreenSelLock(uint8 second);
extern void DrawScreenSelAngle(uint16 angle);

extern void DrawScreenSelOK();
extern void DrawScreenSelCancel();

extern void DrawScreenEditLock(uint8 second);
extern void DrawScreenEditAngle(uint16 angle);

//calibration
extern void DrawCaliMenu();
extern void DrawCaliNormalVoltage();
extern void DrawCaliNormalCurrent();

extern void DrawCaliSelVoltage();
extern void DrawCaliSelCurrent();

extern void DrawCaliNormalBack();
extern void DrawCaliSelBack();

//voltage calibration
extern void DrawVoltageCaliMenu();

extern void DrawVoltageCaliRef(uint8 dec, uint16 frac);

extern void DrawVoltageCaliNormalStart();
extern void DrawVoltageCaliNormalCancel();

extern void DrawVoltageCaliSelStart();
extern void DrawVoltageCaliSelCancel();

extern void DrawVoltageCaliSelOK();
extern void DrawVoltageCaliSelDone();

extern void DrawVoltageCaliProgressBar();
extern void DrawVoltageCaliProgressStep(uint8 from, uint8 to);
extern void ClearVoltageCalProcess();

//current calibration
extern void DrawCurrentCaliMenu();

extern void DrawCurrentCaliRef(uint8 dec, uint16 frac);

extern void DrawCurrentCaliNormalStart();
extern void DrawCurrentCaliNormalCancel();

extern void DrawCurrentCaliSelStart();
extern void DrawCurrentCaliSelCancel();

extern void DrawCurrentCaliSelOK();
extern void DrawCurrentCaliSelDone();

extern void DrawCurrentCaliProgressBar();
extern void DrawCurrentCaliProgressStep(uint8 from, uint8 to);
extern void ClearCurrentCalProcess();

//ble
extern void DrawBleMenu();

extern void DrawBleNormalName(const uint8 *name);
extern void DrawBleNormalSwitch(bool on);

extern void DrawBleNormalOK();
extern void DrawBleNormalCancel();

extern void DrawBleSelSwitch(bool on);

extern void DrawBleSelOK();
extern void DrawBleSelCancel();

extern void DrawBleEditSwitch(bool on);

//version
extern void DrawVersionMenu();

//ble com mode
extern void DrawBleComMenu();
extern void DrawBleComBtActive();
extern void DrawBleComBtInActive();

//message
extern void DrawMessageCaption(const uint8 *caption, uint16 fc);
extern void DrawMessageMsg(uint16 x, uint16 y, const uint8 *msg);

extern void DrawMessageNormalOK();
extern void DrawMessageSelOK();

extern void DrawMessageNormalCancel();
extern void DrawMessageSelCancel();

extern void DrawMessageNormalYes();
extern void DrawMessageNormalNo();

extern void DrawMessageSelYes();
extern void DrawMessageSelNo();

#endif

