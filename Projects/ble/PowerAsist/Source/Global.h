
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

typedef enum
{
	SNIFFER_NONE = 0,

	SNIFFER_QC_20,

	SNIFFER_QC_30,

	SNIFFER_PD,
} SnifferStatus;

extern SnifferStatus GetCurrentSnifferStatus();
extern void SetCurrentSnifferStatus(SnifferStatus status);

extern MENU_ID GetMainMenu(uint8 index);

extern uint8 GetNextMainMenuIndex(uint8 curIndex);

extern uint8 GetPrevMainMenuIndex(uint8 curIndex);

#endif

