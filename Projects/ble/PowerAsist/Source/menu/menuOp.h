
#ifndef _MENU_OP_H_
#define _MENU_OP_H_

/* menu messages */
typedef enum
{
	MSG_ID_CREATE,

	MSG_ID_DESTROY,

	MSG_ID_KEY,

	MSG_ID_TIMEOUT,

	MSG_ID_BLE_STATUS_CHANGED,
	
	MSG_ID_BLE_DATA_RECEIVED,
} MSG_ID;

/* menus */
typedef enum
{
	MENU_ID_WELCOME = 0,

	MENU_ID_MAIN,
	MENU_ID_DETAIL,
	MENU_ID_SLEEP,

	MENU_ID_LOCK,
	
	MENU_ID_SNIFFER,
	MENU_ID_QC20,
	MENU_ID_QC30,
	MENU_ID_PD_LIST,
	MENU_ID_PD,
	MENU_ID_AUTO,
	
	MENU_ID_SETTING,
	MENU_ID_TIME,
	MENU_ID_SAMPLE,
	MENU_ID_SCREEN,
	MENU_ID_CALI,
	MENU_ID_BLE,
	MENU_ID_VERSION,

	MENU_ID_BLE_COMM,
	
	MENU_ID_MESSAGE,
	
	MENU_ID_COUNT,
	MENU_ID_CURRENT,

	MENU_ID_NONE
} MENU_ID;

typedef struct
{
	uint8 *buf;
	uint8 len;
} BleData;

#define DEF_MENU_HANDLER(menuId)    void Menu##menuId##Handler(MSG_ID msgId, void *msg)

#define BEGIN_MENU_HANDLER(menuId)  DEF_MENU_HANDLER(menuId)  { switch (msgId) {

#define ON_MENU_CREATE()            case MSG_ID_CREATE: \
										OnMenuCreate((MENU_ID)(uint16)msg); \
										break;
										
#define ON_MENU_DESTROY()           case MSG_ID_DESTROY: \
										OnMenuDestroy((MENU_ID)(uint16)msg); \
										break;
										
#define ON_MENU_KEY()               case MSG_ID_KEY: \
										{            \
										uint16 key = (uint16)msg; \
										OnMenuKey(key & 0xff, (key >> 8)); \
										}            \
										break;

#define ON_MENU_TIMEOUT()           case MSG_ID_TIMEOUT: \
										OnMenuTimeout((uint16)msg); \
										break;

#define ON_MENU_BLE_STATUS_CHANGED() case MSG_ID_BLE_STATUS_CHANGED: \
										OnMenuBleStatusChanged((uint16)msg); \
										break;

#define ON_MENU_BLE_DATA_RECEIVED() case MSG_ID_BLE_DATA_RECEIVED: \
										{                          \
										BleData *data = (BleData *)msg; \
										OnMenuBleDataReceived(data->buf, data->len); \
										}                          \
										break;

#define END_MENU_HANDLER()          default: break; }}

/* menu functions */
/* welcome */
DEF_MENU_HANDLER(MENU_ID_WELCOME);

/* main */
DEF_MENU_HANDLER(MENU_ID_MAIN);

/* detail */
DEF_MENU_HANDLER(MENU_ID_DETAIL);

/* sleep */
DEF_MENU_HANDLER(MENU_ID_SLEEP);

/* lock */
DEF_MENU_HANDLER(MENU_ID_LOCK);

/* sniffer */
DEF_MENU_HANDLER(MENU_ID_SNIFFER);

/* qc2.0 */
DEF_MENU_HANDLER(MENU_ID_QC20);

/* qc3.0 */
DEF_MENU_HANDLER(MENU_ID_QC30);

/* usb pd list */
DEF_MENU_HANDLER(MENU_ID_PD_LIST);

/* usb pd */
DEF_MENU_HANDLER(MENU_ID_PD);

/* auto detect */
DEF_MENU_HANDLER(MENU_ID_AUTO);

/* setting */
DEF_MENU_HANDLER(MENU_ID_SETTING);

/* time */
DEF_MENU_HANDLER(MENU_ID_TIME);

/* sample */
DEF_MENU_HANDLER(MENU_ID_SAMPLE);

/* screen */
DEF_MENU_HANDLER(MENU_ID_SCREEN);

/* cali */
DEF_MENU_HANDLER(MENU_ID_CALI);

/* ble */
DEF_MENU_HANDLER(MENU_ID_BLE);

/* version */
DEF_MENU_HANDLER(MENU_ID_VERSION);

/* ble comm */
DEF_MENU_HANDLER(MENU_ID_BLE_COMM);

/* message */
DEF_MENU_HANDLER(MENU_ID_MESSAGE);

#endif

