
#include "comdef.h"
#include "hal_types.h"

#include "RTC.h"

#include "ISL1208.h"

TimeStruct g_defaultTime = 
{
	.second = 0,
	.minute = 0,
	.hour   = 0,
	.year   = 2000,
	.month  = 1,
	.day    = 1,
};

static uint8 IntToBcd(uint8 d)
{
	return ((d / 10) << 4) + ((d % 10) & 0x0f);
}

static uint8 BcdToInt(uint8 bcd)
{
	return ((bcd >> 4) & 0x0f) * 10 + (bcd & 0x0f);
}

bool SetRTCTime(const TimeStruct *time)
{
	if (time->second > 59
		|| time->minute > 59
		|| time->hour > 23
		|| time->day < 1 || time->day > 31
		|| time->month < 1 || time->month > 12 
		|| time->dayOfWeek > 6
		|| time->year < 2000 || time->year > 2000 + 99)
	{
		return false;
	}
	
	uint8 dat[7];
	dat[0] = IntToBcd(time->second);
	dat[1] = IntToBcd(time->minute);
	dat[2] = IntToBcd(time->hour) | 0x80;
	dat[3] = IntToBcd(time->day);
	dat[4] = IntToBcd(time->month);
	dat[5] = IntToBcd(time->year - 2000);
	dat[6] = IntToBcd(time->dayOfWeek);

	//enable write rtc
	if (!WriteISL1208Byte(ISL1208_REG_SR, 0x90))
	{
		return false;
	}
	
	bool res = WriteISL1208Data(ISL1208_REG_SC, dat, sizeof(dat));

	//disable write rtc
	WriteISL1208Byte(ISL1208_REG_SR, 0x80);

	return res;
}

bool GetRTCTime(TimeStruct *time)
{
	if (time != NULL)
	{
		uint8 dat[7];
		if (!ReadISL1208Data(ISL1208_REG_SC, dat, sizeof(dat), NULL))
		{
			return false;
		}
		
		time->second    = BcdToInt(dat[0]);
		time->minute    = BcdToInt(dat[1]);
		time->hour      = BcdToInt(dat[2] & 0x3f);
		time->day       = BcdToInt(dat[3]);
		time->month     = BcdToInt(dat[4]);
		time->year      = BcdToInt(dat[5]) + 2000;
		time->dayOfWeek = BcdToInt(dat[6]);

		return true;
	}

	return false;
}

