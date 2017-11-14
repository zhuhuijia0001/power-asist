
#ifndef _AUTODETECT_H_
#define _AUTODETECT_H_

typedef enum
{
	detect_type_dcp = 0,
	detect_type_qc20,
	detect_type_qc20_9v,
	detect_type_qc20_12v,
	detect_type_qc20_20v,
	detect_type_qc30,
	detect_type_apple_21a,
	detect_type_pd,
	detect_type_none,
} DetectType;

extern void StartAutoDetect(void (*callback)(DetectType type, bool support));

extern void AutoDetectTimeout();

#endif

