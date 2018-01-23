
#ifndef _MENUMESSAGE_H_
#define _MENUMESSAGE_H_

//message type
#define MSG_TYPE_OK        (1 << 0)
#define MSG_TYPE_OK_CANCEL (1 << 1)
#define MSG_TYPE_YES_NO    (1 << 2)

#define MSG_TYPE_WARNING   (1 << 4)
#define MSG_TYPE_INFORMATION (1 << 5)

//message result
#define MESSAGE_OK         0

#define MESSAGE_CANCEL     1
#define MESSAGE_YES        2
#define MESSAGE_NO         3

extern void EnterMessageMenu(uint16 x, uint16 y, const uint8 *msg, const uint8 *caption, uint8 msgType, void (*resultCallback)(uint8 result));

#endif

