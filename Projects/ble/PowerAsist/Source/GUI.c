
#include "hal_types.h"

#include "GUI.h"
#include "font.h"

const PICTURE g_Logo = 
{
	.width = 90,
	.height = 77,
	.data = gImage_logo,
};

void DrawPoint(uint16 x, uint16 y, uint16 color)
{
	SetLcdRegion(x, y, x, y);
	WriteLcdData16Bit(color);
}

void DrawLine(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint16 color)   
{
	int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	
    
	dx = x1-x0;
	dy = y1-y0;

	if (dx == 0)
	{
		if (y1 < y0)
		{
			uint16 t = y1;
			y1 = y0;
			y0 = t;
		}

		SetLcdRegion(x0, y0, x1, y1);

		uint16 count = y1 - y0 + 1;
		do 
		{
			WriteLcdData16Bit(color);
		} while (--count);

		return;
	}

	if (dy == 0)
	{
		if (x1 < x0)
		{
			uint16 t = x1;
			x1 = x0;
			x0 = t;
		}

		SetLcdRegion(x0, y0, x1, y1);

		uint16 count = x1 - x0 + 1;
		do 
		{
			WriteLcdData16Bit(color);
		} while (--count);

		return;
	}
	
	if (dx > 0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy > 0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)
	{
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//¨°a?-¦Ì?¦Ì?¨ºy2??¨¢3?1yx?¨¤¨¤?
		{
			DrawPoint(x0, y0, color);
			
			// test if error has overflowed
			if (error >= 0) //¨º?¡¤?D¨¨¨°a???¨®y¡Á?¡À¨º?¦Ì
			{
				error-=dx2;

				// move to next line
				y0+=y_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;
		} // end for
	} // end if |slope| <= 1
	else
	{
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			DrawPoint(x0, y0, color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}

void FillRectangle(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint16 color)
{
	if (x0 > x1)
	{
		uint16 tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1)
	{
		uint16 tmp = y0;
		y0 = y1;
		y1 = tmp;
	}
	
	SetLcdRegion(x0, y0, x1, y1);

	uint16 count = (x1 - x0 + 1) * (y1 - y0 + 1);
	do
	{
		WriteLcdData16Bit(color);
	} while (--count);
}

void DrawRectangle(uint16 x0, uint16 y0, uint16 x1, uint16 y1, uint16 color)
{
	DrawLine(x0, y0, x1, y0, color);
	DrawLine(x0, y1, x1, y1, color);
	DrawLine(x0, y0 + 1, x0, y1 - 1, color);
	DrawLine(x1, y0 + 1, x1, y1 - 1, color);
}

void ClearScreen(uint16 color)               
{	
	uint8 width = GetLcdWidth();
	uint8 height = GetLcdHeight();
	
	FillRectangle(0, 0, width - 1, height - 1, color);
}

static void DrawString16(uint16 x, uint16 y, const uint8 *s, uint16 fc, uint16 bc)
{
	uint8 i;
	uint16 k;

	uint16 width = GetScreenWidth();
	
	const uint8 *dat;
	
	while (*s) 
	{	
		k = *s++;
		if (k == '\n')
		{
			x = 0;
			y += 16;

			continue;
		}
		else if (k >= '0' && k <= '9')
		{
			dat = s_digit8X16[k - '0'];
		}
		else if (k >= 'A' && k <= 'Z')
		{
			dat = s_upperLetter8X16[k - 'A'];
		}
		else if (k >= 'a' && k <= 'z')
		{
			dat = s_lowerLetter8X16[k - 'a'];
		}
		else if (k == '.')
		{
			dat = s_dot8X16;
		}
		else if (k == '+')
		{
			dat = s_plus8X16;
		}
		else if (k == '-')
		{
			dat = s_minus8X16;
		}
		else if (k == ':')
		{
			dat = s_colon8X16;
		}
		else if (k == '/')
		{
			dat = s_slash8X16;
		}
		else if (k == ' ')
		{
			dat = s_space8X16;
		}
		else
		{
			dat = NULL;
		}
		
		if (dat != NULL)
		{
			SetLcdRegion(x, y, x + 8 - 1, y + 16 - 1);

			for (i = 0; i < 8 / 8 * 16; i++)
			{
				uint8 j;
				uint8 mask = 0x80;
				uint8 c = dat[i];
				for (j = 0; j < 8; j++)
				{
					if (c & mask)
					{
						WriteLcdData16Bit(fc);
					}
					else 
					{
						WriteLcdData16Bit(bc);
					}

					mask >>= 1;
				}
			}
	
			x += 8;	

			if (x >= width)
			{
				x = 0;

				y += 16;
			}
		}
	}
}

static void DrawString20(uint16 x, uint16 y, const uint8 *s, uint16 fc, uint16 bc)
{
	uint8 i;
	uint16 k;

	uint16 width = GetScreenWidth();
	
	const uint8 *dat;
	uint8 step;
	
	while (*s) 
	{	
		k = *s++;
		if (k == '\n')
		{
			x = 0;
			y += 20;

			continue;
		}
		else if (k >= '0' && k <= '9')
		{
			dat = s_digit16X20[k - '0'];

			step = 16;
		}
		else if (k >= 'A' && k <= 'Z')
		{
			dat = s_upperLetter16X20[k - 'A'];

			step = 16;
		}
		else if (k >= 'a' && k <= 'z')
		{
			dat = s_lowerLetter16X20[k - 'a'];

			step = 16;
		}
		else if (k == '.')
		{
			dat = s_dot8X20;

			step = 8;
		}
		else if (k == '+')
		{
			dat = s_plus8X20;

			step = 8;
		}
		else if (k == '-')
		{
			dat = s_minus8X20;

			step = 8;
		}
		else if (k == ':')
		{
			dat = s_colon8X20;

			step = 8;
		}
		else if (k == ' ')
		{
			dat = s_space16X20;

			step = 16;
		}
		else
		{
			dat = NULL;
		}
		
		if (dat != NULL)
		{
			uint8 byteCount;
			
			if (step == 16)
			{
				SetLcdRegion(x, y, x + 16 - 1, y + 20 - 1);

				byteCount = 16 / 8 * 20;
			}
			else
			{
				SetLcdRegion(x, y, x + 8 - 1, y + 20 - 1);

				byteCount = 8 / 8 * 20;
			}

			for (i = 0; i < byteCount; i++)
			{
				uint8 j;
				uint8 mask = 0x80;
				uint8 c = dat[i];
				for (j = 0; j < 8; j++)
				{
					if (c & mask)
					{
						WriteLcdData16Bit(fc);
					}
					else 
					{
						WriteLcdData16Bit(bc);
					}

					mask >>= 1;
				}
			}
	
			x += step;	

			if (x >= width)
			{
				x = 0;
				y += 20;
			}
		}
	}

}

static void DrawString24(uint16 x, uint16 y, const uint8 *s, uint16 fc, uint16 bc)
{
	uint8 i;
	uint16 k;

	uint16 width = GetScreenWidth();
	
	const uint8 *dat;
	uint8 step;
	
	while (*s) 
	{	
		k = *s++;
		if (k == '\n')
		{
			x = 0;
			y += 24;

			continue;
		}
		else if (k >= '0' && k <= '9')
		{
			dat = s_digit16X24[k - '0'];

			step = 16;
		}
		else if (k >= 'A' && k <= 'Z')
		{
			dat = s_upperLetter16X24[k - 'A'];

			step = 16;
		}
		else if (k >= 'a' && k <= 'z')
		{
			dat = s_lowerLetter16X24[k - 'a'];

			step = 16;
		}
		else if (k == ' ')
		{
			dat = s_space16X24;

			step = 16;
		}
		else if (k == '.')
		{
			dat = s_dot8X24;

			step = 8;
		}
		else if (k == '+')
		{
			dat = s_plus8X24;

			step = 8;
		}
		else if (k == '-')
		{
			dat = s_minus8X24;

			step = 8;
		}
		else
		{
			dat = NULL;
		}
		
		if (dat != NULL)
		{
			uint8 byteCount;
			
			if (step == 16)
			{
				SetLcdRegion(x, y, x + 16 - 1, y + 24 - 1);

				byteCount = 16 / 8 * 24;
			}
			else
			{
				SetLcdRegion(x, y, x + 8 - 1, y + 24 - 1);

				byteCount = 8 / 8 * 24;
			}

			for (i = 0; i < byteCount; i++)
			{
				uint8 j;
				uint8 mask = 0x80;
				uint8 c = dat[i];
				for (j = 0; j < 8; j++)
				{
					if (c & mask)
					{
						WriteLcdData16Bit(fc);
					}
					else 
					{
						WriteLcdData16Bit(bc);
					}

					mask >>= 1;
				}
			}
	
			x += step;

			if (x >= width)
			{
				x = 0;
				y += 24;
			}
		}
	}
}

static void (*const s_drawStringFun[])(uint16 x, uint16 y, const uint8 *s, uint16 fc, uint16 bc) = 
{
	[font_16] = DrawString16,
	[font_20] = DrawString20,
	[font_24] = DrawString24,
};

void DrawString(FONT font, uint16 x, uint16 y, const uint8 *s, uint16 fc, uint16 bc)
{
	if (font >= font_count)
	{
		return;
	}

	s_drawStringFun[font](x, y, s, fc, bc);
}

static uint16 GetString16Width(const uint8 *s)
{
	uint16 width = 0;
	
	while (*s++)
	{
		width += 8;
	}

	return width;
}

static uint16 GetString20Width(const uint8 *s)
{
	uint16 width = 0;
	uint8 k;
	while (*s)
	{
		k = *s++;

		if (k >= '0' && k <= '9')
		{
			width += 16;
		}
		else if (k >= 'A' && k <= 'Z')
		{
			width += 16;
		}
		else if (k >= 'a' && k <= 'z')
		{
			width += 16;
		}
		else if (k == '.')
		{
			width += 8;
		}
		else if (k == '+')
		{
			width += 8;
		}
		else if (k == '-')
		{
			width += 8;
		}
		else if (k == ':')
		{
			width += 8;
		}
		else if (k == ' ')
		{
			width += 16;
		}
	}

	return width;
}

static uint16 GetString24Width(const uint8 *s)
{
	uint16 width = 0;
	uint8 k;
	while (*s)
	{
		k = *s++;
		
		if (k >= '0' && k <= '9')
		{
			width += 16;
		}
		else if (k >= 'A' && k <= 'Z')
		{
			width += 16;
		}
		else if (k >= 'a' && k <= 'z')
		{
			width += 16;
		}
		else if (k == ' ')
		{
			width += 16;
		}
		else if (k == '.')
		{
			width += 8;
		}
		else if (k == '+')
		{
			width += 8;
		}
		else if (k == '-')
		{
			width += 8;
		}
	}

	return width;
}

static uint16 (*const s_getStringWidthFun[])(const uint8 *s) = 
{
	[font_16] = GetString16Width,
	[font_20] = GetString20Width,
	[font_24] = GetString24Width,
};

uint16 GetStringWidth(FONT font, const uint8 *s)
{
	if (font >= font_count)
	{
		return 0;
	}

	return s_getStringWidthFun[font](s);
}

void DrawPicture(uint16 x, uint16 y, const PICTURE *pic)
{
	uint8 width = pic->width;
	uint8 height = pic->height;
	const uint8 *data = pic->data;
	
	SetLcdRegion(x, y, x + width - 1, y + height - 1);

	uint16 count = width * height;
	do
	{
		uint16 val = (*data++ << 8);
		val |= *data++;
		WriteLcdData16Bit(val);
	} while (--count);
}

void DrawPicturePart(uint16 x, uint16 y, uint16 offsetx, uint16 offsety, 
						uint16 width, uint16 height, const PICTURE *pic)
{
	const uint8 *data = pic->data + (offsety * pic->width + offsetx) * sizeof(uint16);
	
	SetLcdRegion(x + offsetx, y + offsety, x + offsetx + width - 1, y + offsety + height - 1);

	uint16 delta = (pic->width - width) * sizeof(uint16);
	uint16 i, j;
	for (i = 0; i < height; i++)
	{		
		j = width;
		do
		{
			uint16 val = (*data++ << 8);
			val |= *data++;
			WriteLcdData16Bit(val);
		} while (--j);

		data += delta;
	}
}

