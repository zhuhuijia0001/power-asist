
#ifndef _QUICKCHARGE_H_
#define _QUICKCHARGE_H_

extern void InitQC();

extern void StartQC20Sniffer(void (*callback)(bool supported));

extern void CheckQC20Supported();

extern void StopQC20Sniffer();

extern void SetQC20Sniffer5V();

extern void SetQC20Sniffer9V();

extern void SetQC20Sniffer12V();

extern void SetQC20Sniffer20V();

extern void SetQC30Mode();

extern void IncreaseQC30Voltage();

extern void DecreaseQC30Voltage();

#endif

