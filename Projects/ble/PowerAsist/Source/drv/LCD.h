
#ifndef _LCD_H_
#define _LCD_H_

#define COLOR_24B_TO_16B(r, g, b)      ((((r) >> 3) << (5 + 6))   \
										| (((g) >> 2) << 5)        \
										| (((b) >> 3) << 0))
//color
#define WHITE	COLOR_24B_TO_16B(255, 255, 255)
#define BLACK	COLOR_24B_TO_16B(0, 0, 0)
#define RED  	COLOR_24B_TO_16B(255, 0, 0)
#define BLUE 	COLOR_24B_TO_16B(0, 0, 255)
#define MAGENTA COLOR_24B_TO_16B(255, 0, 255)
#define GREEN	COLOR_24B_TO_16B(0, 255, 0)
#define CYAN    COLOR_24B_TO_16B(0, 255, 255)
#define YELLOW  COLOR_24B_TO_16B(255, 255, 0)

#define ORANGE  COLOR_24B_TO_16B(255, 128, 64)

#define GRAY0   0xEF7D 
#define GRAY1   0x8410 
#define GRAY2   0x4208

//orientation
typedef enum
{
	ORIENTATION_0 = 0,
	ORIENTATION_90,
	ORIENTATION_180,
	ORIENTATION_270,

	ORIENTATION_COUNT
} Orientation;

extern void InitLCD(Orientation orient);

extern void WriteLcdData16Bit(uint16 dat);

extern void EnableLcdBacklight(bool enable);

extern void SetLcdRegion(uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end);

extern void SetLcdOrientation(Orientation orient);

extern uint8 GetLcdWidth();

extern uint8 GetLcdHeight();

extern uint8 GetScreenWidth();

extern uint8 GetScreenHeight();

#endif

