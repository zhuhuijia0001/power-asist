
#ifndef _REALTIMEDATA_H_
#define _REALTIMEDATA_H_

typedef struct
{
	uint32 valh;
	
	uint32 sum;
} AccumulateStruct;

extern void SaveInitRealTimeData(const AccumulateStruct *Wh, const AccumulateStruct *Ah);

extern void SaveRealTimeData(const AccumulateStruct *Wh, const AccumulateStruct *Ah);

extern void ReadRealTimeData(AccumulateStruct *Wh, AccumulateStruct *Ah);

#endif

