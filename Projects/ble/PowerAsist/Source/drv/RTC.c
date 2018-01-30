
#include "comdef.h"
#include "hal_types.h"

#include "RTC.h"

#include "ISL1208.h"

//default time
static TimeStruct s_defaultTime = 
{
	.second = 0,
	.minute = 0,
	.hour   = 0,
	.year   = 2000,
	.month  = 1,
	.day    = 1,
	.dayOfWeek = 0,
};

static uint8 IntToBcd(uint8 d)
{
	return ((d / 10) << 4) + ((d % 10) & 0x0f);
}

static uint8 BcdToInt(uint8 bcd)
{ 
	return ((bcd >> 4) & 0x0f) * 10 + (bcd & 0x0f);
}

bool SetDefaultRTCTime(void)
{
	return SetRTCTime(&s_defaultTime);
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
	
	uint8 dat[ISL1208_RTC_SECTION_LEN];
	dat[ISL1208_REG_SC] = IntToBcd(time->second);
	dat[ISL1208_REG_MN] = IntToBcd(time->minute);
	dat[ISL1208_REG_HR] = IntToBcd(time->hour) | ISL1208_REG_HR_MIL;
	dat[ISL1208_REG_DT] = IntToBcd(time->day);
	dat[ISL1208_REG_MO] = IntToBcd(time->month);
	dat[ISL1208_REG_YR] = IntToBcd(time->year - 2000);
	dat[ISL1208_REG_DW] = IntToBcd(time->dayOfWeek & 7);

	//enable write rtc
	bool res = WriteISL1208Byte(ISL1208_REG_SR, ISL1208_REG_SR_ARST | ISL1208_REG_SR_WRTC);
	if (!res)
	{
		return false;
	}
	
	res = WriteISL1208Data(ISL1208_REG_SC, dat, sizeof(dat));
	if (!res)
	{
		return false;
	}

	//disable write rtc
	res = WriteISL1208Byte(ISL1208_REG_SR, ISL1208_REG_SR_ARST);

	return res;
}

bool GetRTCTime(TimeStruct *time)
{
	if (time != NULL)
	{
		uint8 dat[ISL1208_RTC_SECTION_LEN];
		if (!ReadISL1208Data(ISL1208_REG_SC, dat, sizeof(dat), NULL))
		{
			return false;
		}
		
		time->second = BcdToInt(dat[ISL1208_REG_SC]);
		time->minute = BcdToInt(dat[ISL1208_REG_MN]);
		if (dat[ISL1208_REG_HR] & ISL1208_REG_HR_MIL)
		{
			/* 24h format */
			time->hour = BcdToInt(dat[ISL1208_REG_HR] & 0x3f);
		}
		else
		{
			/* 12h format */
			time->hour = BcdToInt(dat[ISL1208_REG_HR] & 0x1f);
			if (dat[ISL1208_REG_HR] & ISL1208_REG_HR_PM)
			{
				/* PM flag */
				time->hour += 12;
			}
		}
	
		time->day       = BcdToInt(dat[ISL1208_REG_DT]);
		time->month     = BcdToInt(dat[ISL1208_REG_MO]);
		time->year      = BcdToInt(dat[ISL1208_REG_YR]) + 2000;
		time->dayOfWeek = BcdToInt(dat[ISL1208_REG_DW]);

		return true;
	}

	return false;
}

