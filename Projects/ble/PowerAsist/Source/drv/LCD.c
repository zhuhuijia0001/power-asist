#include "comdef.h"
#include "hal_types.h"

#include "PinDefine.h"

#include "LCD.h"
#include "Delay.h"

// UxUCR - USART UART Control Register.
#define UCR_FLUSH                  0x80

#undef  UxCSR
#undef  UxUCR
#undef  UxDBUF
#undef  UxBAUD
#undef  UxGCR

#define UxCSR                      U1CSR
#define UxUCR                      U1UCR
#define UxDBUF                     U1DBUF
#define UxBAUD                     U1BAUD
#define UxGCR                      U1GCR

#undef  PxSEL
#undef  HAL_UART_PERCFG_BIT
#undef  HAL_UART_PRIPO
#undef  HAL_UART_Px_SEL_S
#undef  HAL_UART_Px_SEL_M

#define PxSEL                      P0SEL
#define HAL_UART_PERCFG_BIT        0x02         // USART1 on P1, Alt-1; so set this bit.
#define HAL_UART_Px_SEL_M          0x18         // Peripheral I/O Select for Master: /MO/CLK.

//lcd dimension
#define LANDSCAPE_MAX_WIDTH       160
#define LANDSCAPE_MAX_HEIGHT      128

#define PORTRAIT_MAX_WIDTH        128
#define PORTRAIT_MAX_HEIGHT       160

//screen size
#define SCREEN_WIDTH             128
#define SCREEN_HEIGHT            128

static Orientation s_curOrientation = ORIENTATION_0;

static void InitLcdGPIO()
{
	SET_GPIO_OUTPUT(PIN_LCD_LAMP);
	CLR_GPIO_BIT(PIN_LCD_LAMP);
	
	SET_GPIO_OUTPUT(PIN_LCD_RESET);
	SET_GPIO_OUTPUT(PIN_LCD_DC);

	PERCFG |= 0x01;// push away uart0
	PERCFG &= ~HAL_UART_PERCFG_BIT;

	PxSEL |= HAL_UART_Px_SEL_M;        // SPI-Master peripheral select.
	UxCSR = 0;                         // Mode is SPI-Master Mode.
	UxGCR = 19;                        // Cfg for the max Rx/Tx baud.
	UxBAUD = 255;

	UxUCR = UCR_FLUSH;                 // Flush it.
	UxGCR |= (BV(7) | BV(6) | BV(5));          // Set bit order to MSB.
}

static void WriteCmd(uint8 cmd)
{
	CLR_GPIO_BIT(PIN_LCD_DC);

	UxDBUF = cmd;
	while (UxCSR & (1 << 0));
}

static void WriteData(uint8 dat)
{
	SET_GPIO_BIT(PIN_LCD_DC);
	
	UxDBUF = dat;
	while (UxCSR & (1 << 0));
}

void WriteLcdData16Bit(uint16 dat)
{
	SET_GPIO_BIT(PIN_LCD_DC);
	
	UxDBUF = dat >> 8;
	while (UxCSR & (1 << 0));

	UxDBUF = dat;
	while (UxCSR & (1 << 0));
}

void ResetLCD()
{
	CLR_GPIO_BIT(PIN_LCD_RESET);
	DelayMs(50);

	SET_GPIO_BIT(PIN_LCD_RESET);
	DelayMs(50);
}

void InitLCD(Orientation orient)
{
	s_curOrientation = orient;
	
	InitLcdGPIO();
	EnableLcdBacklight(false);
	
	ResetLCD(); //Reset before LCD Init.

	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	WriteCmd(0x11);//Sleep exit 
	DelayMs(120);
		
	//ST7735R Frame Rate
	WriteCmd(0xB1); 
	WriteData(0x01); 
	WriteData(0x2C); 
	WriteData(0x2D); 

	WriteCmd(0xB2); 
	WriteData(0x01); 
	WriteData(0x2C); 
	WriteData(0x2D); 

	WriteCmd(0xB3); 
	WriteData(0x01); 
	WriteData(0x2C); 
	WriteData(0x2D); 
	WriteData(0x01); 
	WriteData(0x2C); 
	WriteData(0x2D); 
	
	WriteCmd(0xB4); //Column inversion 
	WriteData(0x07); 
	
	//ST7735R Power Sequence
	WriteCmd(0xC0); 
	WriteData(0xA2); 
	WriteData(0x02); 
	WriteData(0x84); 
	WriteCmd(0xC1); 
	WriteData(0xC5); 

	WriteCmd(0xC2); 
	WriteData(0x0A); 
	WriteData(0x00); 

	WriteCmd(0xC3); 
	WriteData(0x8A); 
	WriteData(0x2A); 
	WriteCmd(0xC4); 
	WriteData(0x8A); 
	WriteData(0xEE); 
	
	WriteCmd(0xC5); //VCOM 
	WriteData(0x0E); 

	SetLcdOrientation(s_curOrientation);
	
	//ST7735R Gamma Sequence
	WriteCmd(0xe0); 
	WriteData(0x0f); 
	WriteData(0x1a); 
	WriteData(0x0f); 
	WriteData(0x18); 
	WriteData(0x2f); 
	WriteData(0x28); 
	WriteData(0x20); 
	WriteData(0x22); 
	WriteData(0x1f); 
	WriteData(0x1b); 
	WriteData(0x23); 
	WriteData(0x37); 
	WriteData(0x00); 	
	WriteData(0x07); 
	WriteData(0x02); 
	WriteData(0x10); 

	WriteCmd(0xe1); 
	WriteData(0x0f); 
	WriteData(0x1b); 
	WriteData(0x0f); 
	WriteData(0x17); 
	WriteData(0x33); 
	WriteData(0x2c); 
	WriteData(0x29); 
	WriteData(0x2e); 
	WriteData(0x30); 
	WriteData(0x30); 
	WriteData(0x39); 
	WriteData(0x3f); 
	WriteData(0x00); 
	WriteData(0x07); 
	WriteData(0x03); 
	WriteData(0x10);  
	/*
	WriteCmd(0x2a);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(X_MAX_PIXEL - 1);

	WriteCmd(0x2b);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(Y_MAX_PIXEL - 1);
	
	WriteCmd(0xF0); //Enable test command  
	WriteData(0x01); 
	*/
	WriteCmd(0xF6); //Disable ram power save mode 
	WriteData(0x00); 
	
	WriteCmd(0x3A); //65k mode 
	WriteData(0x05); 

	WriteCmd(0x29); //Display on
}

void EnableLcdBacklight(bool enable)
{
	if (enable)
	{
		SET_GPIO_BIT(PIN_LCD_LAMP);
	}
	else
	{
		CLR_GPIO_BIT(PIN_LCD_LAMP);
	}
}

void SetLcdRegion(uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end)
{
#define ORIENTATION_0_X_OFFSET     3
#define ORIENTATION_0_Y_OFFSET     2

#define ORIENTATION_90_X_OFFSET     2
#define ORIENTATION_90_Y_OFFSET     0

#define ORIENTATION_180_X_OFFSET   1
#define ORIENTATION_180_Y_OFFSET   2

#define ORIENTATION_270_X_OFFSET     2
#define ORIENTATION_270_Y_OFFSET     3

	uint8 x_offset;
	uint8 y_offset;
	if (s_curOrientation == ORIENTATION_0)
	{
		x_offset = ORIENTATION_0_X_OFFSET;
		y_offset = ORIENTATION_0_Y_OFFSET;
	}
	else if (s_curOrientation == ORIENTATION_90)
	{
		x_offset = ORIENTATION_90_X_OFFSET;
		y_offset = ORIENTATION_90_Y_OFFSET;
	}
	else if (s_curOrientation == ORIENTATION_180)
	{
		x_offset = ORIENTATION_180_X_OFFSET;
		y_offset = ORIENTATION_180_Y_OFFSET;
	}
	else
	{
		x_offset = ORIENTATION_270_X_OFFSET;
		y_offset = ORIENTATION_270_Y_OFFSET;
	}
	
	WriteCmd(0x2a);
	WriteData(0x00);
	WriteData(x_start + x_offset);
	WriteData(0x00);
	WriteData(x_end + x_offset);

	WriteCmd(0x2b);
	WriteData(0x00);
	WriteData(y_start + y_offset);
	WriteData(0x00);
	WriteData(y_end + y_offset);
	
	WriteCmd(0x2c);
}

void SetLcdOrientation(Orientation orient)
{
	switch (orient)
	{
	case ORIENTATION_0:
		WriteCmd(0x36);
		WriteData(0xA8);

		break;

	case ORIENTATION_90:
		WriteCmd(0x36);
		WriteData(0x08);

		break;

	case ORIENTATION_180:
		WriteCmd(0x36);
		WriteData(0x68);

		break;

	case ORIENTATION_270:
		WriteCmd(0x36);
		WriteData(0xC8);

		break;
	}
}

uint8 GetLcdWidth()
{
	if (s_curOrientation == ORIENTATION_0 || s_curOrientation == ORIENTATION_180)
	{
		return LANDSCAPE_MAX_WIDTH;
	}
	else
	{
		return PORTRAIT_MAX_WIDTH;
	}
}

uint8 GetLcdHeight()
{
	if (s_curOrientation == ORIENTATION_0 || s_curOrientation == ORIENTATION_180)
	{
		return LANDSCAPE_MAX_HEIGHT;
	}
	else
	{
		return PORTRAIT_MAX_HEIGHT;
	}
}

uint8 GetScreenWidth()
{
	return SCREEN_WIDTH;
}

uint8 GetScreenHeight()
{
	return SCREEN_HEIGHT;
}

