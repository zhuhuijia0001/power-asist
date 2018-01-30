
#ifndef _RTC_H_
#define _RTC_H_

//time struct
typedef struct
{
  uint8 second;  // 0-59
  uint8 minute;  // 0-59
  uint8 hour;     // 0-23
  uint8 day;      // 1-31
  uint8 month;    // 1-12
  uint8 dayOfWeek; //0-6
  uint16 year;    // 2000+
} TimeStruct;

extern bool SetDefaultRTCTime(void);

extern bool SetRTCTime(const TimeStruct *time);

extern bool GetRTCTime(TimeStruct *time);

#endif

