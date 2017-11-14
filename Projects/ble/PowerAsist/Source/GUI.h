
#ifndef _GUI_H_
#define _GUI_H_

#include "LCD.h"

typedef struct
{
	uint8 width;
	uint8 height;

	const uint8 *data;
} PICTURE;

typedef enum
{
	font_16 = 0,
	font_20,
	font_24,

	font_count,	
} FONT;

extern const PICTURE g_Logo;

extern void DrawPoint(uint16 x, uint16 y, uint16 color);

extern void DrawLine(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint16 color) ;

extern void FillRectangle(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint16 color);

extern void DrawRectangle(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint16 color);

extern void ClearScreen(uint16 color);

extern void DrawString(FONT font, uint16 x, uint16 y, const uint8 *s, uint16 fc, uint16 bc);

extern uint16 GetStringWidth(FONT font, const uint8 *s);

extern void DrawPicture(uint16 x, uint16 y, const PICTURE *pic);

extern void DrawPicturePart(uint16 x, uint16 y, uint16 offsetx, uint16 offsety, 
						uint16 width, uint16 height, const PICTURE *pic);
						
#endif

