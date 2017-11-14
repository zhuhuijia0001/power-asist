
#ifndef _PACKET_H_
#define _PACKET_H_

typedef struct
{
	uint16 dec;

	uint16 frac;
} DataItem;

typedef struct
{
	uint8 voltage; //V

	uint16 current; //mA
} PDItem;

#define PACKET_OK              0
#define PACKET_NOT_COMPLETE    1
#define PACKET_INVALID         2

extern uint8 BuildTestDataPacket(uint8 *buf, uint8 maxBufLen, 
										uint8 mode, uint8 modeVoltage,
										uint8 dataBitmap, 
										const DataItem *data);

extern uint8 BuildEnableQc20RetPacket(uint8 *buf, uint8 maxBufLen, uint8 enable, uint8 result);

extern uint8 BuildEnableQc30RetPacket(uint8 *buf, uint8 maxBufLen, uint8 enable, uint8 result);

extern uint8 BuildEnablePdRetPacket(uint8 *buf, uint8 maxBufLen, uint8 enable, uint8 result, 
									const PDItem *pd, uint8 itemCount);

extern uint8 BuildTriggerRetPacket(uint8 *buf, uint8 maxBufLen, 
									uint8 mode, uint8 modeVoltage,
									uint8 result);

extern uint8 BuildAutoDetectRetPacket(uint8 *buf, uint8 maxBufLen,
										uint8 mode, uint8 modeVoltage,
										uint8 support);

extern uint8 BuildSetTimeRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result);

extern uint8 BuildQueryParamRetPacket(uint8 *buf, uint8 maxBufLen, 
										uint8 sampleRate, uint8 peakDuration,
										const uint8 bleName[MAX_BLE_NAME_LEN],
										const uint8 version[FIRMWARE_VER_LEN]);

extern uint8 BuildSetSampleRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result);

extern uint8 BuildSetBleNameRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result);

extern uint8 ParsePacket(const uint8 *buf, uint8 len, uint8 *parsedLen);

extern void ResetParsePacket();

extern const uint8 *GetPacketData();

extern uint8 GetPacketType();
extern uint8 GetPacketDataLen();

extern bool ParseEnableTransferPacket(const uint8 *data, uint8 len, uint8 *dataBitmap);
extern bool ParseEnableQc20Packet(const uint8 *data, uint8 len, uint8 *enable);
extern bool ParseEnableQc30Packet(const uint8 *data, uint8 len, uint8 *enable);
extern bool ParseEnablePdPacket(const uint8 *data, uint8 len, uint8 *enable);

extern bool ParseTriggerPacket(const uint8 *data, uint8 len, uint8 *mode, uint8 *modeVoltage);

extern bool ParseSetTimePacket(const uint8 *data, uint8 len, TimeStruct *time);

extern bool ParseSetSamplePacket(const uint8 *data, uint8 len, uint8 *sampleRate,
									uint8 *peakDuration);

extern bool ParseSetBleNamePacket(const uint8 *data, uint8 len, uint8 bleName[MAX_BLE_NAME_LEN]);

#endif
