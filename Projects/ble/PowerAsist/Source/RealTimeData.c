#include "comdef.h"
#include "hal_types.h"

#include "EEPROM.h"

#include "RealTimeData.h"

#include "npi.h"

#define MEMBER_OFFSET(type, member)     ( (uint16)&(((type *)0)->member))

#define REAL_TIME_DATA_ADDR_A       1024

#define REAL_TIME_DATA_ADDR_B       (REAL_TIME_DATA_ADDR_A + 128)

/********************************************
 *  Save region
 *  Wh data,   Ah data,   data id,   ok flag
 *******************************************/

#define MAX_DATA_ID       0xAAAA

#define SAVE_DATA_OK      0xA5A5
#define SAVE_DATA_INVALID 0xFFFF

static const uint16 s_invalidFlag = SAVE_DATA_INVALID;

//real time data store vector
typedef struct
{
	uint32 WhVal;
	uint32 WSum;

	uint32 AhVal;
	uint32 ASum;

	uint16 dataId;

	uint16 saveFlag;
} RealTimeStoreVector;

static uint16  s_dataId = 0;

void SaveInitRealTimeData(const AccumulateStruct *Wh, const AccumulateStruct *Ah)
{
	RealTimeStoreVector vector;

	//save data
	vector.WhVal = Wh->valh;
	vector.WSum = Wh->sum;

	vector.AhVal = Ah->valh;
	vector.ASum = Ah->sum;

	//save id
	s_dataId = 0;
	vector.dataId = s_dataId;

	//save ok flag
	vector.saveFlag = SAVE_DATA_OK;

	WriteEEPROMData(REAL_TIME_DATA_ADDR_A, (uint8 *)&vector, sizeof(vector));
	WriteEEPROMData(REAL_TIME_DATA_ADDR_B + MEMBER_OFFSET(RealTimeStoreVector, saveFlag), (uint8 *)&s_invalidFlag, sizeof(s_invalidFlag));
}

void SaveRealTimeData(const AccumulateStruct *Wh, const AccumulateStruct *Ah)
{
	uint16 addr;
	
	RealTimeStoreVector vector;

	//save data
	vector.WhVal = Wh->valh;
	vector.WSum = Wh->sum;

	vector.AhVal = Ah->valh;
	vector.ASum = Ah->sum;

	//save id
	vector.dataId = s_dataId;

	//TRACE("Wh val:%lu\r\n", Wh->valh);
	//TRACE("Wh sum:%lu\r\n", Wh->sum);
	
	//TRACE("Ah val:%lu\r\n", Ah->valh);
	//TRACE("Ah sum:%lu\r\n", Ah->sum);
	
	if (!(s_dataId & 0x01))
	{
		addr = REAL_TIME_DATA_ADDR_A;

		//TRACE("save on A\r\n");
	}
	else
	{
		addr = REAL_TIME_DATA_ADDR_B;

		//TRACE("save on B\r\n");
	}
	
	s_dataId++;
	if (s_dataId == MAX_DATA_ID)
	{
		s_dataId = 0;
	}

	//clear OK flag first
	WriteEEPROMData(addr + MEMBER_OFFSET(RealTimeStoreVector, saveFlag), (uint8 *)&s_invalidFlag, sizeof(s_invalidFlag));

	//save ok flag
	vector.saveFlag = SAVE_DATA_OK;
	WriteEEPROMData(addr, (uint8 *)&vector, sizeof(vector));
}

void ReadRealTimeData(AccumulateStruct *Wh, AccumulateStruct *Ah)
{	
	RealTimeStoreVector vectorA;	
	RealTimeStoreVector vectorB;

	ReadEEPROMData(REAL_TIME_DATA_ADDR_A, (uint8 *)&vectorA, sizeof(vectorA), NULL);
	ReadEEPROMData(REAL_TIME_DATA_ADDR_B, (uint8 *)&vectorB, sizeof(vectorB), NULL);

	if (vectorA.saveFlag == SAVE_DATA_OK
		&& vectorB.saveFlag == SAVE_DATA_OK)
	{
		TRACE("both save flag ok\r\n");

		if (vectorA.dataId > vectorB.dataId
			|| (vectorA.dataId == 0 && vectorB.dataId == MAX_DATA_ID - 1))
		{
			TRACE("A is better\r\n");
			TRACE("Wh val:%lu,Wh sum:%lu\r\n", vectorA.WhVal, vectorA.WSum);
			TRACE("Ah val:%lu,Ah sum:%lu\r\n", vectorA.AhVal, vectorA.ASum);
			
			if (Wh != NULL)
			{
				Wh->valh = vectorA.WhVal;
				Wh->sum = vectorA.WSum;
			}

			if (Ah != NULL)
			{
				Ah->valh = vectorA.AhVal;
				Ah->sum = vectorA.ASum;
			}
		}
		else
		{
			TRACE("B is better\r\n");
			TRACE("Wh val:%lu,Wh sum:%lu\r\n", vectorB.WhVal, vectorB.WSum);
			TRACE("Ah val:%lu,Ah sum:%lu\r\n", vectorB.AhVal, vectorB.ASum);
			if (Wh != NULL)
			{
				Wh->valh = vectorB.WhVal;
				Wh->sum = vectorB.WSum;
			}

			if (Ah != NULL)
			{
				Ah->valh = vectorB.AhVal;
				Ah->sum = vectorB.ASum;
			}
		}
	}
	else if (vectorA.saveFlag == SAVE_DATA_OK)
	{
		TRACE("A save flag ok\r\n");
		
		if (Wh != NULL)
		{
			Wh->valh = vectorA.WhVal;
			Wh->sum = vectorA.WSum;
		}

		if (Ah != NULL)
		{
			Ah->valh = vectorA.AhVal;
			Ah->sum = vectorA.ASum;
		}
	}
	else if (vectorB.saveFlag == SAVE_DATA_OK)
	{
		TRACE("B save flag ok\r\n");
		
		if (Wh != NULL)
		{
			Wh->valh = vectorB.WhVal;
			Wh->sum = vectorB.WSum;
		}

		if (Ah != NULL)
		{
			Ah->valh = vectorB.AhVal;
			Ah->sum = vectorB.ASum;
		}
	}
	else
	{
		TRACE("impossible\r\n");

		if (Wh != NULL)
		{
			Wh->valh = 0;
			Wh->sum = 0;
		}

		if (Ah != NULL)
		{
			Ah->valh = 0;
			Ah->sum = 0;
		}
	}
}

