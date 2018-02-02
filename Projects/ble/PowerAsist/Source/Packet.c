#include "hal_types.h"
#include "comdef.h"

#include "Parameter.h"
#include "Protocol.h"

#include "RTC.h"
#include "Packet.h"

#include "Aes.h"

#include "npi.h"

static uint8 s_recvBuf[MAX_PACKET_LEN];

typedef enum
{
	recv_status_idle = 0,
	recv_status_version,
	recv_status_length,
	recv_status_data,
} BleReceiveStatus;

static BleReceiveStatus s_recvStatus = recv_status_idle;

static uint8 s_dataRecvIndex = 0;

static uint8 *s_aesKey = NULL;

typedef enum
{
	packet_normal = 0,

	packet_encrypt,
} packet_type;

static packet_type s_packetType = packet_normal;

void SetPacketAesKey(uint8 aesKey[AES_KEY_LEN])
{
	if (aesKey == NULL)
	{
		s_packetType = packet_normal;
	}
	else
	{
		s_packetType = packet_encrypt;
	}

	s_aesKey = aesKey;
}

static uint8 BuildPacket(uint8 *buf, uint8 maxBufLen, const uint8 *data, uint8 len)
{
	if (1 + 1 + 2 + len + 1 > maxBufLen)
	{
		return 0xff;
	}

	uint8 index = 0;

	buf[index++] = PACKET_HEADER;

	buf[index++] = PROTOCOL_VERSION;
	
	buf[index++] = (len + 1) >> 8;
	buf[index++] = (len + 1);
	
	uint8 sum = 0;
	for (uint8 i = 0; i < len; i++)
	{
		buf[index++] = *data;
		
		sum ^= *data;

		data++;
	}
	buf[index++] = sum;

	return index;
}

static uint8 BuildPacketWithKey(uint8 *buf, uint8 maxBufLen, const uint8 *data, uint8 len)
{
	uint8 realDataLen = (len + 1 + AES_KEY_LEN - 1) & ~(AES_KEY_LEN - 1);
	if (1 + 1 + 2 + realDataLen > maxBufLen)
	{
		return 0xff;
	}

	uint8 index = 0;

	buf[index++] = PACKET_HEADER;

	buf[index++] = PROTOCOL_VERSION;
	
	buf[index++] = realDataLen >> 8;
	buf[index++] = realDataLen;

	uint8 plainData[MAX_PACKET_DATA_LEN];
	uint8 encryptData[MAX_PACKET_DATA_LEN];
	
	uint8 sum = 0;
	for (uint8 i = 0; i < len; i++)
	{
		plainData[i] = *data;
		
		sum ^= *data;

		data++;
	}
	plainData[len] = sum;
	for (uint8 i = len + 1; i < realDataLen; i++)
	{
		//padding
		plainData[i] = 0x00;
	}

	//encrypt
	AesEncryptData(s_aesKey, plainData, realDataLen, encryptData);
	
	for (uint8 i = 0; i < realDataLen; i++)
	{
		buf[index++] = encryptData[i];
	}

	return index;
}

static uint8 (*const s_buildPacketFun[])(uint8 *buf, uint8 maxBufLen, const uint8 *data, uint8 len) = 
{
	[packet_normal] = BuildPacket,

	[packet_encrypt] = BuildPacketWithKey,
};

uint8 BuildTestDataPacket(uint8 *buf, uint8 maxBufLen,
										uint8 mode, uint8 modeVoltage,
										uint8 dataBitmap, 
										const DataItem *data)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_TRANSFER_DATA;
	dataBuf[index++] = mode;

	dataBuf[index++] = modeVoltage;

	dataBuf[index++] = dataBitmap;

	if (dataBitmap & DATA_VOLTAGE_MASK)
	{
		dataBuf[index++] = data->dec >> 8;
		dataBuf[index++] = data->dec & 0xff;

		dataBuf[index++] = data->frac >> 8;
		dataBuf[index++] = data->frac & 0xff;

		data++;
	}

	if (dataBitmap & DATA_CURRENT_MASK)
	{
		dataBuf[index++] = data->dec >> 8;
		dataBuf[index++] = data->dec & 0xff;

		dataBuf[index++] = data->frac >> 8;
		dataBuf[index++] = data->frac & 0xff;

		data++;
	}

	if (dataBitmap & DATA_POWER_MASK)
	{
		dataBuf[index++] = data->dec >> 8;
		dataBuf[index++] = data->dec & 0xff;

		dataBuf[index++] = data->frac >> 8;
		dataBuf[index++] = data->frac & 0xff;

		data++;
	}

	if (dataBitmap & DATA_WH_MASK)
	{
		dataBuf[index++] = data->dec >> 8;
		dataBuf[index++] = data->dec & 0xff;

		dataBuf[index++] = data->frac >> 8;
		dataBuf[index++] = data->frac & 0xff;

		data++;
	}

	if (dataBitmap & DATA_AH_MASK)
	{
		dataBuf[index++] = data->dec >> 8;
		dataBuf[index++] = data->dec & 0xff;

		dataBuf[index++] = data->frac >> 8;
		dataBuf[index++] = data->frac & 0xff;

		data++;
	}

	if (dataBitmap & DATA_DP_MASK)
	{
		dataBuf[index++] = data->dec >> 8;
		dataBuf[index++] = data->dec & 0xff;

		dataBuf[index++] = data->frac >> 8;
		dataBuf[index++] = data->frac & 0xff;

		data++;
	}

	if (dataBitmap & DATA_DM_MASK)
	{
		dataBuf[index++] = data->dec >> 8;
		dataBuf[index++] = data->dec & 0xff;

		dataBuf[index++] = data->frac >> 8;
		dataBuf[index++] = data->frac & 0xff;

		data++;
	}

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildEnableQc20RetPacket(uint8 *buf, uint8 maxBufLen, uint8 enable, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_ENABLE_QC20;
	dataBuf[index++] = enable;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildEnableQc30RetPacket(uint8 *buf, uint8 maxBufLen, uint8 enable, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_ENABLE_QC30;
	dataBuf[index++] = enable;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildEnablePdRetPacket(uint8 *buf, uint8 maxBufLen, uint8 enable, uint8 result, 
									const PDItem *pd, uint8 itemCount)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_ENABLE_PD;
	dataBuf[index++] = enable;
	dataBuf[index++] = result;
	dataBuf[index++] = itemCount;

	for (uint8 i = 0; i < itemCount; i++)
	{
		dataBuf[index++] = pd[i].voltage;

		dataBuf[index++] = pd[i].current >> 8;
		dataBuf[index++] = pd[i].current & 0xff;
	}

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildTriggerRetPacket(uint8 *buf, uint8 maxBufLen,
									uint8 mode, uint8 modeVoltage,
									uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_TRIGGER_QUICK_CHARGE;
	dataBuf[index++] = mode;
	dataBuf[index++] = modeVoltage;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildAutoDetectRetPacket(uint8 *buf, uint8 maxBufLen, 
										uint8 mode, uint8 modeVoltage,
										uint8 support)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_AUTO_DETECT;
	dataBuf[index++] = mode;
	dataBuf[index++] = modeVoltage;
	dataBuf[index++] = support;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildSetTimeRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_SET_TIME;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildQueryParamRetPacket(uint8 *buf, uint8 maxBufLen, 
										uint8 sampleRate, uint8 peakDuration,
										const uint8 version[FIRMWARE_VER_LEN])
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_QUERY_PARAM;
	dataBuf[index++] = sampleRate;
	dataBuf[index++] = peakDuration;
	
	for (uint8 i = 0; i < FIRMWARE_VER_LEN; i++)
	{
		dataBuf[index++] = version[i];
	}

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildSetSampleRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_SET_SAMPLE;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildSetPeakDurationRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_SET_PEAK_DURATION;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildSetBleNameRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_SET_BLE_NAME;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildQueryChargeModeRetPacket(uint8 *buf, uint8 maxBufLen, uint8 mode, uint8 voltage, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_QUERY_CHARGE_MODE;
	dataBuf[index++] = mode;
	dataBuf[index++] = voltage;
	dataBuf[index++] = result;

	return s_buildPacketFun[s_packetType](buf, maxBufLen, dataBuf, index);
}

uint8 BuildTransferAesKeyRetPacket(uint8 *buf, uint8 maxBufLen, uint8 result)
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_TRANSFER_AES_KEY;
	dataBuf[index++] = result;

	return BuildPacket(buf, maxBufLen, dataBuf, index);
}

uint8 BuildTestAesKeyRetPacket(uint8 *buf, uint8 maxBufLen, const uint8 cypherText[AES_TEST_DATA_LEN])
{
	uint8 dataBuf[MAX_PACKET_LEN];
	
	uint8 index = 0;
	dataBuf[index++] = TYPE_TEST_AES_KEY;
	
	for (uint8 i = 0; i < AES_TEST_DATA_LEN; i++)
	{
		dataBuf[index++] = cypherText[i];
	}

	return BuildPacket(buf, maxBufLen, dataBuf, index);
}

uint8 ParsePacket(const uint8 *buf, uint8 len, uint8 *parsedLen)
{
	static uint16 length = 0;
	static uint16 dataLeft = 0;

	uint8 count = 0;
	
	uint8 res = PACKET_NOT_COMPLETE;
	
	for (uint8 i = 0; i < len; i++)
	{
		uint8 dat = *buf++;
		count++;
		
		switch (s_recvStatus)
		{
		case recv_status_idle:
			if (dat == PACKET_HEADER)
			{
				s_recvStatus = recv_status_version;
			}
			
			break;
	
		case recv_status_version:
			if (dat == PROTOCOL_VERSION)
			{
				s_recvStatus = recv_status_length;

				dataLeft = 2;
			}
			else
			{
				res = PACKET_INVALID;

				s_recvStatus = recv_status_idle;
			}
			
			break;
	
		case recv_status_length:
			length <<= 8;
			length |= dat;
			dataLeft--;
			if (dataLeft == 0)
			{
				if (length > MAX_PACKET_LEN - 4)
				{
					res = PACKET_INVALID;

					TRACE("packet invalid\r\n");
				}
				else
				{
					s_recvStatus = recv_status_data;

					dataLeft = length;

					s_dataRecvIndex = 0;
				}
			}
			
			break;

		case recv_status_data:
			s_recvBuf[s_dataRecvIndex++] = dat;
			dataLeft--;
			
			if (dataLeft == 0)
			{
				s_recvStatus = recv_status_idle;

				res = PACKET_OK;
			}
	
			break;			
		}

		if (res == PACKET_OK || res == PACKET_INVALID)
		{
			break;
		}
	}

	if (parsedLen != NULL)
	{
		*parsedLen = count;
	}
	
	return res;
}

void ResetParsePacket()
{
	s_recvStatus = recv_status_idle;
	
	s_dataRecvIndex = 0;
}

uint8 *GetPacketData()
{
	return s_recvBuf;
}

uint8 GetPacketType()
{
	return s_recvBuf[0];
}

uint8 GetPacketDataLen()
{
	return s_dataRecvIndex;
}

static uint8 PacketDataLen(uint8 type)
{
	uint8 len = 1;

	switch (type)
	{
	case TYPE_ENABLE_TRANSFER:
		len += SIZE_ENABLE_TRANSFER;

		break;

	case TYPE_ENABLE_QC20:
	case TYPE_ENABLE_QC30:
	case TYPE_ENABLE_PD:
		len += SIZE_ENABLE_QUICK_CHARGE;

		break;

	case TYPE_TRIGGER_QUICK_CHARGE:
		len += SIZE_ENABLE_TRIGGER;

		break;

	case TYPE_SET_TIME:
		len += SIZE_SET_TIME;

		break;

	case TYPE_SET_SAMPLE:
		len += SIZE_SET_SAMPLE;

		break;

	case TYPE_SET_BLE_NAME:	
		len += SIZE_SET_BLE_NAME;

		break;

	case TYPE_SET_PEAK_DURATION:
		len += SIZE_SET_PEAK_DURATION;

		break;

	}

	return len;
}

void DecryptPacketData()
{
	AesDecryptData(s_aesKey, s_recvBuf, s_dataRecvIndex, s_recvBuf);

	uint8 type = s_recvBuf[0];
	
	s_dataRecvIndex = PacketDataLen(type);
}

static bool CheckPacketDataSum(const uint8 *packetData, uint8 len)
{
	uint8 sum = 0;
	len--;
	for (uint8 i = 0; i < len; i++)
	{	
		sum ^= packetData[i];
	}
	
	if (sum != packetData[len])
	{
		return false;
	}

	return true;
}

bool ParseEnableTransferPacket(const uint8 *data, uint8 len, uint8 *dataBitmap)
{
	if (len != SIZE_ENABLE_TRANSFER + 1)
	{
		return false;
	}
	
	uint8 index = 0;

	uint8 dat;

	if (!CheckPacketDataSum(data, len))
	{
		return false;
	}

	index++;
	dat = data[index++];
	if (dataBitmap != NULL)
	{
		*dataBitmap = dat;
	}

	return true;
}

static bool ParseEnableQuickChargePacket(const uint8 *data, uint8 len, uint8 *enable)
{
	if (len != SIZE_ENABLE_QUICK_CHARGE + 1)
	{
		return false;
	}
	
	uint8 index = 0;
	
	uint8 dat;

	if (!CheckPacketDataSum(data, len))
	{
		return false;
	}

	index++;
	dat = data[index++];
	if (enable != NULL)
	{
		*enable = dat;
	}

	return true;
}

bool ParseEnableQc20Packet(const uint8 *data, uint8 len, uint8 *enable)
{
	return ParseEnableQuickChargePacket(data, len, enable);
}

bool ParseEnableQc30Packet(const uint8 *data, uint8 len, uint8 *enable)
{
	return ParseEnableQuickChargePacket(data, len, enable);
}

bool ParseEnablePdPacket(const uint8 *data, uint8 len, uint8 *enable)
{
	return ParseEnableQuickChargePacket(data, len, enable);
}

bool ParseTriggerPacket(const uint8 *data, uint8 len, uint8 *mode, uint8 *modeVoltage)
{
	if (len != SIZE_ENABLE_TRIGGER + 1)
	{
		return false;
	}
	
	uint8 index = 0;
	
	uint8 dat;

	if (!CheckPacketDataSum(data, len))
	{
		return false;
	}

	index++;
	dat = data[index++];
	if (mode != NULL)
	{
		*mode = dat;
	}

	dat = data[index++];
	if (modeVoltage != NULL)
	{
		*modeVoltage = dat;
	}
	
	return true;
}

bool ParseSetTimePacket(const uint8 *data, uint8 len, TimeStruct *time)
{
	if (len != SIZE_SET_TIME + 1)
	{
		return false;

	}
	
	uint8 index = 0;
	
	uint8 dat;

	if (!CheckPacketDataSum(data, len))
	{
		return false;
	}

	index++;
	if (time != NULL)
	{
		//year
		dat = data[index++];
		time->year = 2000 + dat;
	
		//month
		dat = data[index++]; 
		time->month = dat;
	
		//day
		dat = data[index++];
		time->day = dat;
	
		//hour
		dat = data[index++];
		time->hour = dat;
	
		//minute
		dat = data[index++];
		time->minute = dat;
	
		//second
		dat = data[index++];
		time->second = dat;
	
		//day of week is default(0)
		time->dayOfWeek = 0;
	}
	
	return true;
}

bool ParseSetSamplePacket(const uint8 *data, uint8 len, uint8 *sampleRate)
{
	if (len != SIZE_SET_SAMPLE + 1)
	{
		return false;
	}
	
	uint8 index = 0;
		
	uint8 dat;

	if (!CheckPacketDataSum(data, len))
	{
		return false;
	}
	
	index++;

	//sample rate
	dat = data[index++];
	if (sampleRate != NULL)
	{
		*sampleRate = dat;
	}

	return true;
}

bool ParseSetPeakDurationPacket(const uint8 *data, uint8 len, uint8 *peakDuration)
{
	if (len != SIZE_SET_PEAK_DURATION + 1)
	{
		return false;
	}
	
	uint8 index = 0;
		
	uint8 dat;

	if (!CheckPacketDataSum(data, len))
	{
		return false;
	}
	
	index++;

	//peakDuration
	dat = data[index++];
	if (peakDuration != NULL)
	{
		*peakDuration = dat;
	}

	return true;
}

bool ParseSetBleNamePacket(const uint8 *data, uint8 len, uint8 bleName[MAX_BLE_NAME_LEN])
{	
	if (len != SIZE_SET_BLE_NAME + 1)
	{
		return false;
	}
	
	uint8 index = 0;

	if (!CheckPacketDataSum(data, len))
	{
		return false;
	}
	
	index++;

	for (uint8 i = 0; i < MAX_BLE_NAME_LEN; i++)
	{
		bleName[i] = data[index++];
	}

	return true;
}

bool ParseTransferAesKeyPacket(const uint8 *data, uint8 len, uint8 aesKey[AES_KEY_LEN])
{
	if (len != SIZE_TRANSFER_AES_KEY + 1)
	{
		return false;
	}
	
	uint8 index = 0;

	index++;

	for (uint8 i = 0; i < AES_KEY_LEN; i++)
	{
		aesKey[i] = data[index++];
	}

	return true;
}

bool ParseTestAesKeyPacket(const uint8 *data, uint8 len, uint8 cypherText[AES_TEST_DATA_LEN])
{
	if (len != SIZE_TEST_AES_KEY + 1)
	{
		return false;
	}
	
	uint8 index = 0;

	index++;

	for (uint8 i = 0; i < AES_TEST_DATA_LEN; i++)
	{
		cypherText[i] = data[index++];
	}

	return true;
}

