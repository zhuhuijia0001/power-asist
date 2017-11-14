
#ifndef _MENUMESSAGE_H_
#define _MENUMESSAGE_H_

#define MESSAGE_YES        0
#define MESSAGE_NO         1

extern void EnterMessageMenu(uint16 x, uint16 y, const uint8 *msg, void (*resultCallback)(uint8 result));

#endif

