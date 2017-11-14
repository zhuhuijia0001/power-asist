
#ifndef _MENU_H_
#define _MENU_H_

extern void SwitchToMenu(MENU_ID id);

extern MENU_ID GetCurrentMenu();

extern void ProcessCurMenuKey(uint8 key, uint8 type);

extern void ProcessMenuTimeout(uint16 timerId);

extern void ProcessMenuBleStatusChanged(uint16 status);

extern void ProcessMenuBleDataReceived(uint8 *buf, uint8 len);

extern void InitMenu();

#endif

