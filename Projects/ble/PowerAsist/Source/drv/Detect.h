
#ifndef _DETECT_H_
#define _DETECT_H_

typedef enum
{
	d_type_dcp = 0,
	d_type_apple_21A,
	d_type_other,
} DType;

extern void StartDetectDType(void (*callback)(DType type));

extern void CheckDType();

#endif

