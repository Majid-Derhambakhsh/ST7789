/*
------------------------------------------------------------------------------
~ File   : st7789.c
~ Author : Majid Derhambakhsh
~ Version: V1.0.0
~ Created: 08/25/2023 18:00:00 PM
~ Brief  :
~ Support:
           E-Mail : Majid.Derhambakhsh@gmail.com (subject : Embedded Library Support)
           
           Github : https://github.com/Majid-Derhambakhsh
------------------------------------------------------------------------------
~ Description:    

~ Attention  :    This library uses RGB565 as color format, you can use ST7789_Color_GetFromRGB and 
                  ST7789_Color_GetFromHex functions to convert RGB/Hex color format to it.
                  
				  You can use ST7789_PutImage to show the image in the display, 
                  Use https://lvgl.io/tools/imageconverter to convert your image to the supported
                  array by the ST7789_PutImage function.
                  
                  Convert Setup:
                  - Color format  -> CF_RGB565A8
                  - Output format -> C array
                  - Enable 'Output in big-endian format' Option
                  
~ Changes    :    
------------------------------------------------------------------------------
*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "st7789.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ G Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifdef ST7789_USE_DMA

#define LCD_BUFFER_LENGTH (ST7789_WIDTH_MODIFIED * ST7789_HOR_LEN)

uint16_t LCDBuffer[LCD_BUFFER_LENGTH];

#endif

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ G Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ G Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ G Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void ST7789_TransmitCommand(ST7789_CMDTypeDef CMD)
{
	
	/* ---------------- Select Chip ----------------- */
	ST7789_GPIO_WritePin(ST7789_CS_GPIO_PORT, ST7789_CS_GPIO_PIN, ST7789_GPIO_PIN_RESET);
	
	/* ------------- Set to Command Mode ------------ */
	ST7789_GPIO_WritePin(ST7789_DC_GPIO_PORT, ST7789_DC_GPIO_PIN, ST7789_GPIO_PIN_RESET);
	
	/* ---------------- Transmit Data --------------- */
	ST7789_SPI_Transmit(&CMD, 1, ST7789_SPI_TIMEOUT);
	
	/* ---------------- Unselect Chip --------------- */
	ST7789_GPIO_WritePin(ST7789_CS_GPIO_PORT, ST7789_CS_GPIO_PIN, ST7789_GPIO_PIN_SET);
	
}

void ST7789_TransmitSingleData(uint8_t Data)
{
	
	/* ---------------- Select Chip ----------------- */
	ST7789_GPIO_WritePin(ST7789_CS_GPIO_PORT, ST7789_CS_GPIO_PIN, ST7789_GPIO_PIN_RESET);
	
	/* -------------- Set to Data Mode -------------- */
	ST7789_GPIO_WritePin(ST7789_DC_GPIO_PORT, ST7789_DC_GPIO_PIN, ST7789_GPIO_PIN_SET);
	
	/* ---------------- Transmit Data --------------- */
	ST7789_SPI_Transmit(&Data, 1, ST7789_SPI_TIMEOUT);
	
	/* ---------------- Unselect Chip --------------- */
	ST7789_GPIO_WritePin(ST7789_CS_GPIO_PORT, ST7789_CS_GPIO_PIN, ST7789_GPIO_PIN_SET);
	
	
}

void ST7789_TransmitData(uint8_t *Data, uint32_t Size)
{
	
	uint16_t tSize;
	
	/* ---------------- Select Chip ----------------- */
	ST7789_GPIO_WritePin(ST7789_CS_GPIO_PORT, ST7789_CS_GPIO_PIN, ST7789_GPIO_PIN_RESET);
	
	/* -------------- Set to Data Mode -------------- */
	ST7789_GPIO_WritePin(ST7789_DC_GPIO_PORT, ST7789_DC_GPIO_PIN, ST7789_GPIO_PIN_SET);
	
	/* ---------------- Transmit Data --------------- */	
	while (Size > 0)
	{
		
		/* ~~~~~~~~~~~~~~~ Calculate transmit size ~~~~~~~~~~~~~~~ */
		tSize = Size > UINT16_MAX ? UINT16_MAX : Size;
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		#ifdef ST7789_USE_DMA
		
		if (ST7789_DMA_MIN_SIZE <= Size)
		{
			ST7789_SPI_Transmit_DMA(Data, tSize);
			while (ST7789_SPI.State != ST7789_SPI_STATE_READY) { }
			//while (ST7789_SPI.hdmatx->State != ST7789_DMA_STATE_READY) { }
		}
		else
		{
			ST7789_SPI_Transmit(Data, tSize, ST7789_SPI_TIMEOUT);
		}
		
		#else
		
		ST7789_SPI_Transmit(Data, tSize, ST7789_SPI_TIMEOUT);
		
		#endif
		
		Data += tSize;
		Size -= tSize;
		
	}
	
	/* ---------------- Unselect Chip --------------- */
	ST7789_GPIO_WritePin(ST7789_CS_GPIO_PORT, ST7789_CS_GPIO_PIN, ST7789_GPIO_PIN_SET);

}

/* ......................... Initialize ........................ */
void ST7789_Init(void)
{
	
	uint8_t PorchSetting_Default[5] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
	uint8_t PVGAMCTRL_Default[14] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
	uint8_t NVGAMCTRL_Default[14] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
	
	#ifdef ST7789_USE_DMA
	
	memset(LCDBuffer, 0, sizeof(LCDBuffer));
	
	#endif
		
	/* ----------------- Reset Chip ----------------- */
	ST7789_Delay(25);
	ST7789_GPIO_WritePin(ST7789_RST_GPIO_PORT, ST7789_RST_GPIO_PIN, ST7789_GPIO_PIN_RESET);
	ST7789_Delay(25);
	ST7789_GPIO_WritePin(ST7789_RST_GPIO_PORT, ST7789_RST_GPIO_PIN, ST7789_GPIO_PIN_SET);
	ST7789_Delay(50);
	
	/* ---------------------------------------------- */
	ST7789_TransmitCommand(ST7789_CMD_COLMOD); // Set Color Mode
	ST7789_TransmitSingleData(ST7789_COLOR_MODE_16BIT);
	
	ST7789_TransmitCommand(ST7789_CMD_PORCTRL); // Set Porch Control
	ST7789_TransmitData(PorchSetting_Default, 5);
	
	ST7789_SetRotation(ST7789_ROTATION); // Set Display Rotation
	
	/* Internal LCD Voltage generator settings */
	ST7789_TransmitCommand(ST7789_CMD_GCTRL); // Gate Control
	ST7789_TransmitSingleData(ST7789_REG_DEF_GCTRL);
	
	ST7789_TransmitCommand(ST7789_CMD_VCOMS); // VCOM setting
	ST7789_TransmitSingleData(ST7789_REG_DEF_VCOMS); // 0.725v (Default: 0.75v -> 0x20)
	
	ST7789_TransmitCommand(ST7789_CMD_LCMCTRL); // LCM Control
	ST7789_TransmitSingleData(ST7789_REG_DEF_LCMCTRL);
	
	ST7789_TransmitCommand(ST7789_CMD_VDVVRHEN); // VDV and VRH Command Enable
	ST7789_TransmitSingleData(ST7789_REG_DEF_VDVVRHEN);
	
	ST7789_TransmitCommand(ST7789_CMD_VRHS); // VRH set
	ST7789_TransmitSingleData(ST7789_REG_DEF_VRHS); // +-4.45v (Default: +-4.1v -> 0x0B)
	
	ST7789_TransmitCommand(ST7789_CMD_VDVS); // VDV set
	ST7789_TransmitSingleData(ST7789_REG_DEF_VDVS);
	
	ST7789_TransmitCommand(ST7789_CMD_FRCTRL2); // Frame Rate Control in Normal Mode
	ST7789_TransmitSingleData(ST7789_REG_DEF_FRCTRL2); // Default value (60HZ)
	
	ST7789_TransmitCommand(ST7789_CMD_PWCTRL1); // Power control
	ST7789_TransmitSingleData(ST7789_REG_DEF_PWCTRL1H);
	ST7789_TransmitSingleData(ST7789_REG_DEF_PWCTRL1L);
	
	/* Division line */
	ST7789_TransmitCommand(ST7789_CMD_PVGAMCTRL);
	ST7789_TransmitData(PVGAMCTRL_Default, 14);
	
	ST7789_TransmitCommand(ST7789_CMD_NVGAMCTRL);
	ST7789_TransmitData(NVGAMCTRL_Default, 14);
	
	ST7789_TransmitCommand(ST7789_CMD_INVON); // Inversion ON
	ST7789_TransmitCommand(ST7789_CMD_SLPOUT); // Out of sleep mode
	ST7789_TransmitCommand(ST7789_CMD_NORON); // Normal Display on
	ST7789_TransmitCommand(ST7789_CMD_DISPON); // Main screen turned on
	
	ST7789_Delay(50);
	ST7789_FillScreen(ST7789_COLOR_BLACK); // Fill with Black.
	
}

/* .......................... Control .......................... */
void ST7789_SetRotation(uint8_t Rotation)
{
	
	ST7789_TransmitCommand(ST7789_CMD_MADCTL);
	
	switch (Rotation)
	{
		case 0:
		ST7789_TransmitSingleData(ST7789_REG_DEF_MADCTL_MX | ST7789_REG_DEF_MADCTL_MY | ST7789_REG_DEF_MADCTL_RGB);
		break;
		case 1:
		ST7789_TransmitSingleData(ST7789_REG_DEF_MADCTL_MY | ST7789_REG_DEF_MADCTL_MV | ST7789_REG_DEF_MADCTL_RGB);
		break;
		case 2:
		ST7789_TransmitSingleData(ST7789_REG_DEF_MADCTL_RGB);
		break;
		case 3:
		ST7789_TransmitSingleData(ST7789_REG_DEF_MADCTL_MX | ST7789_REG_DEF_MADCTL_MV | ST7789_REG_DEF_MADCTL_RGB);
		break;
		default:
		break;
	}
	
}

void ST7789_SetWindowAddress(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd)
{
	
	XStart = XStart + ST7789_XS;
	XEnd   = XEnd + ST7789_XS;
	
	uint8_t ColumnAddress[4] = {XStart >> 8, XStart & 0xFF, XEnd >> 8, XEnd & 0xFF};
	
	YStart = YStart + ST7789_YS;
	YEnd   = YEnd + ST7789_YS;
	
	uint8_t RowAddress[4] = {YStart >> 8, YStart & 0xFF, YEnd >> 8, YEnd & 0xFF};
	
	/* ------------- Set Column Address ------------- */
	ST7789_TransmitCommand(ST7789_CMD_CASET);
	ST7789_TransmitData(ColumnAddress, sizeof(ColumnAddress));
	
	/* -------------- Set Row Address --------------- */
	ST7789_TransmitCommand(ST7789_CMD_RASET);
	ST7789_TransmitData(RowAddress, sizeof(RowAddress));
	
	/* ------------- Write Param to RAM ------------- */
	ST7789_TransmitCommand(ST7789_CMD_RAMWR);
	
}

void ST7789_InvertColors(ST7789_InvTypeDef Invert)
{
	
	/* -------------- Transmit Command -------------- */
	ST7789_TransmitCommand(Invert ? ST7789_INVERT_ON : ST7789_INVERT_OFF);
	
}

void ST7789_TearEffect(ST7789_ColorModeTypeDef Tear)
{
	
	/* -------------- Transmit Command -------------- */
	ST7789_TransmitCommand(Tear ? ST7789_TEAR_ON : ST7789_TEAR_OFF);
	
}

/* ...................... Color Converting ..................... */
ST7789_ColorTypeDef ST7789_Color_GetFromRGB(uint8_t R, uint8_t G, uint8_t B)
{
	
	/* -------------- Convert to RGB565 ------------- */
	/*
		RGB888 bit Format: RRRRRRRRGGGGGGGGBBBBBBBB
		RGB565 bit Format: RRRRRGGGGGGBBBBB
	*/
	uint16_t rgb565 = (((uint16_t)R & 0xF8) << 8)|(((uint16_t)G & 0xFC) << 3)|((uint16_t)B >> 3);
	
	return rgb565;
	
}

ST7789_ColorTypeDef ST7789_Color_GetFromHex(uint32_t HexCode)
{
	
	/* -------------- Convert to RGB565 ------------- */
	/*
		RGB888 bit Format: RRRRRRRRGGGGGGGGBBBBBBBB
		RGB565 bit Format: RRRRRGGGGGGBBBBB
	*/
	uint16_t rgb565 = (((HexCode & 0xF80000) >> 8) + ((HexCode & 0xFC00) >> 5) + ((HexCode & 0xF8) >> 3));
	
	return rgb565;
	
}

/* ........................... Fill ............................ */
void ST7789_Fill(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd, ST7789_ColorTypeDef Color)
{
	
	uint8_t colorBuff[2] = {Color >> 8, Color & 0xFF};
	
	uint16_t YCounter;
	uint16_t XCounter;
	
	/* ---------------- Size Control ---------------- */
	if ((XEnd < 0) || (XEnd >= ST7789_WIDTH_MODIFIED) || (YEnd < 0) || (YEnd >= ST7789_HEIGHT_MODIFIED))
	{
		return;
	}
	
	ST7789_SetWindowAddress(XStart, YStart, XEnd, YEnd);
	
	/* --------------- Write to Pixels -------------- */
	for (YCounter = YStart; YCounter <= YEnd; YCounter++)
	{
		for (XCounter = XStart; XCounter <= XEnd; XCounter++)
		{
			ST7789_TransmitData(colorBuff, sizeof(colorBuff));
		}
	}
	
}

void ST7789_FillScreen(ST7789_ColorTypeDef Color)
{
	
	uint16_t widthCounter;

	#ifndef ST7789_USE_DMA
	uint16_t heightCounter;
	#endif

	uint16_t packetSize = ST7789_HEIGHT_MODIFIED / ST7789_HOR_LEN;
	
	#ifdef ST7789_USE_DMA

	uint32_t colorCounter;
	uint16_t newColor = (Color & 0xFF) << 8|(Color >> 8);

	#else
	uint8_t colorBuff[2] = {Color >> 8, Color & 0xFF};
	#endif
	
	ST7789_SetWindowAddress(0, 0, ST7789_WIDTH_MODIFIED - 1, ST7789_HEIGHT_MODIFIED - 1);
	
	/* ------------- Transmit LCD Buffer ------------ */
	#ifdef ST7789_USE_DMA
	
	for (colorCounter = 0; colorCounter < (sizeof(LCDBuffer) / 2); colorCounter++)
	{
		LCDBuffer[colorCounter] = newColor;
	}
	
	for (widthCounter = 0; widthCounter < packetSize; widthCounter++)
	{
		ST7789_TransmitData((uint8_t *)LCDBuffer, sizeof(LCDBuffer));
	}

	#else
	
	for (widthCounter = 0; widthCounter < ST7789_WIDTH_MODIFIED; widthCounter++)
	{
		for (heightCounter = 0; heightCounter < ST7789_HEIGHT_MODIFIED; heightCounter++)
		{
			ST7789_TransmitData(colorBuff, sizeof(colorBuff));
		}
	}
	
	#endif
	
}

/* .......................... Drawing .......................... */
void ST7789_DrawPixel(uint16_t XPos, uint16_t YPos, ST7789_ColorTypeDef Color)
{
	
	uint8_t colorBuff[2] = {Color >> 8, Color & 0xFF};
	
	/* ---------------- Size Control ---------------- */
	if ((XPos < 0) || (XPos >= ST7789_WIDTH_MODIFIED) || (YPos < 0) || (YPos >= ST7789_HEIGHT_MODIFIED))
	{
		return;
	}
	
	ST7789_SetWindowAddress(XPos, YPos, XPos, YPos);
	
	/* ---------------- Write Pixel ----------------- */
	ST7789_TransmitData(colorBuff, sizeof(colorBuff));
	
}

void ST7789_DrawPixel_4px(uint16_t XPos, uint16_t YPos, ST7789_ColorTypeDef Color)
{
	
	/* ---------------- Size Control ---------------- */
	if ((XPos <= 0) || (XPos > ST7789_WIDTH_MODIFIED) || (YPos <= 0) || (YPos > ST7789_HEIGHT_MODIFIED))
	{
		return;
	}
	
	/* ---------------- Write Pixel ----------------- */
	ST7789_Fill(XPos - 1, YPos - 1, XPos + 1, YPos + 1, Color);
	
}

void ST7789_DrawLine(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd, ST7789_ColorTypeDef Color)
{
	
	uint16_t Swap;
	uint8_t  Steep = abs(YEnd - YStart) > abs(XEnd - XStart);
	
	int16_t xDif;
	int16_t yDif;
	int16_t err;
	int16_t yStep;
	
	/* ------------------ Swap XY ------------------- */
	if (Steep)
	{
		
		Swap   = XStart;
		XStart = YStart;
		YStart = Swap;
		
		Swap   = XEnd;
		XEnd   = YEnd;
		YEnd   = Swap;
		
	}
	
	/* -------------- Swap XStart/XEnd -------------- */
	if (XStart > XEnd)
	{
		
		Swap   = XStart;
		XStart = XEnd;
		XEnd   = Swap;
		
		Swap   = YStart;
		YStart = YEnd;
		YEnd   = Swap;
		
	}
	
	/* ------------ Calculate Line Length ----------- */
	xDif = XEnd - XStart;
	yDif = abs(YEnd - YStart);
	
	err = xDif / 2;
	
	/* ------------- Modify Y Direction ------------- */
	if (YStart < YEnd)
	{
		yStep = 1;
	}
	else
	{
		yStep = -1;
	}
	
	/* ----------------- Draw Line ------------------ */
	for (; XStart <= XEnd; XStart++)
	{
		
		if (Steep)
		{
			ST7789_DrawPixel(YStart, XStart, Color);
		}
		else
		{
			ST7789_DrawPixel(XStart, YStart, Color);
		}
		
		err -= yDif;
		
		if (err < 0)
		{
			YStart += yStep;
			err    += xDif;
		}
		
	}
	
}

void ST7789_DrawRectangle(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd, ST7789_ColorTypeDef Color)
{
	
	/* ------------ Write Rectangle Lines ----------- */
	ST7789_DrawLine(XStart, YStart, XEnd, YStart, Color);
	ST7789_DrawLine(XStart, YStart, XStart, YEnd, Color);
	ST7789_DrawLine(XStart, YEnd, XEnd, YEnd, Color);
	ST7789_DrawLine(XEnd, YStart, XEnd, YEnd, Color);
	
}

void ST7789_DrawFilledRectangle(uint16_t XPos, uint16_t YPos, uint16_t Width, uint16_t Height, ST7789_ColorTypeDef Color)
{
	
	uint8_t heightCounter;
	
	/* -------------- Position Control -------------- */
	if (XPos >= ST7789_WIDTH_MODIFIED || YPos >= ST7789_HEIGHT_MODIFIED)
	{
		return;
	}
	
	/* ---------------- Size Control ---------------- */
	if ((XPos + Width) >= ST7789_WIDTH_MODIFIED)
	{
		Width = ST7789_WIDTH_MODIFIED - XPos;
	}
	
	if ((YPos + Height) >= ST7789_HEIGHT_MODIFIED)
	{
		Height = ST7789_HEIGHT_MODIFIED - YPos;
	}
	
	/* ----------------- Draw Lines ----------------- */
	for (heightCounter = 0; heightCounter <= Height; heightCounter++)
	{
		ST7789_DrawLine(XPos, YPos + heightCounter, XPos + Width, YPos + heightCounter, Color);
	}
	
}

void ST7789_DrawCircle(uint16_t XPos, uint16_t YPos, uint8_t Radius, ST7789_ColorTypeDef Color)
{
	
	int16_t f = 1 - Radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * Radius;
	int16_t x = 0;
	int16_t y = Radius;
	
	/* ------------ Write Circle Pixels ------------- */
	ST7789_DrawPixel(XPos, YPos + Radius, Color);
	ST7789_DrawPixel(XPos, YPos - Radius, Color);
	ST7789_DrawPixel(XPos + Radius, YPos, Color);
	ST7789_DrawPixel(XPos - Radius, YPos, Color);
	
	while (x < y)
	{
		
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawPixel(XPos + x, YPos + y, Color);
		ST7789_DrawPixel(XPos - x, YPos + y, Color);
		ST7789_DrawPixel(XPos + x, YPos - y, Color);
		ST7789_DrawPixel(XPos - x, YPos - y, Color);

		ST7789_DrawPixel(XPos + y, YPos + x, Color);
		ST7789_DrawPixel(XPos - y, YPos + x, Color);
		ST7789_DrawPixel(XPos + y, YPos - x, Color);
		ST7789_DrawPixel(XPos - y, YPos - x, Color);
		
	}
	
}

void ST7789_DrawFilledCircle(int16_t XPos, int16_t YPos, int16_t Radius, ST7789_ColorTypeDef Color)
{
	
	int16_t f = 1 - Radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * Radius;
	int16_t x = 0;
	int16_t y = Radius;
	
	/* ------------ Write Circle Pixels ------------- */
	ST7789_DrawPixel(XPos, YPos + Radius, Color);
	ST7789_DrawPixel(XPos, YPos - Radius, Color);
	ST7789_DrawPixel(XPos + Radius, YPos, Color);
	ST7789_DrawPixel(XPos - Radius, YPos, Color);
	ST7789_DrawLine(XPos - Radius, YPos, XPos + Radius, YPos, Color);
	
	while (x < y)
	{
		
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		ST7789_DrawLine(XPos - x, YPos + y, XPos + x, YPos + y, Color);
		ST7789_DrawLine(XPos + x, YPos - y, XPos - x, YPos - y, Color);
		
		ST7789_DrawLine(XPos + y, YPos + x, XPos - y, YPos + x, Color);
		ST7789_DrawLine(XPos + y, YPos - x, XPos - y, YPos - x, Color);
		
	}
	
}

void ST7789_DrawTriangle(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t X3, uint16_t Y3, ST7789_ColorTypeDef Color)
{
	
	/* ----------------- Draw Lines ----------------- */
	ST7789_DrawLine(X1, Y1, X2, Y2, Color);
	ST7789_DrawLine(X2, Y2, X3, Y3, Color);
	ST7789_DrawLine(X3, Y3, X1, Y1, Color);
	
}

void ST7789_DrawFilledTriangle(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t X3, uint16_t Y3, ST7789_ColorTypeDef Color)
{
	
	int16_t deltaX    = abs(X2 - X1);
	int16_t deltaY    = abs(Y2 - Y1);
	int16_t xUpdate1  = 0, xUpdate2 = 0;
	int16_t yUpdate1  = 0, yUpdate2 = 0;
	int16_t deltaNum  = 0, num = 0, numToAdd = 0;
	int16_t numPixels = 0, pixelCounter = 0;
	
	/* ----------------- Draw Lines ----------------- */
	if (X2 >= X1)
	{
		xUpdate1 = 1;
		xUpdate2 = 1;
	}
	else
	{
		xUpdate1 = -1;
		xUpdate2 = -1;
	}

	if (Y2 >= Y1)
	{
		yUpdate1 = 1;
		yUpdate2 = 1;
	}
	else
	{
		yUpdate1 = -1;
		yUpdate2 = -1;
	}

	if (deltaX >= deltaY)
	{
		xUpdate1  = 0;
		yUpdate2  = 0;
		deltaNum  = deltaX;
		num       = deltaX / 2;
		numToAdd  = deltaY;
		numPixels = deltaX;
	}
	else
	{
		xUpdate2  = 0;
		yUpdate1  = 0;
		deltaNum  = deltaY;
		num       = deltaY / 2;
		numToAdd  = deltaX;
		numPixels = deltaY;
	}

	for (pixelCounter = 0; pixelCounter <= numPixels; pixelCounter++)
	{
		
		ST7789_DrawLine(X1, Y1, X3, Y3, Color);

		num += numToAdd;
		
		if (num >= deltaNum)
		{
			num -= deltaNum;
			X1  += xUpdate1;
			Y1  += yUpdate1;
		}
		
		X1 += xUpdate2;
		Y1 += yUpdate2;
		
	}
	
}

/* ........................... Image ........................... */
void ST7789_PutImage(uint16_t XPos, uint16_t YPos, uint16_t Width, uint16_t Height, const uint16_t *Image)
{
	
	if ((XPos >= ST7789_WIDTH_MODIFIED) || (YPos >= ST7789_HEIGHT_MODIFIED))
	return;
	
	if ((XPos + Width - 1) >= ST7789_WIDTH_MODIFIED)
	return;
	
	if ((YPos + Height - 1) >= ST7789_HEIGHT_MODIFIED)
	return;
	
	/* -------------- Write Image Data -------------- */
	ST7789_SetWindowAddress(XPos, YPos, XPos + Width - 1, YPos + Height - 1);
	ST7789_TransmitData((uint8_t *)Image, sizeof(uint16_t) * Width * Height);
	
}

/* ........................... Text ............................ */
void ST7789_PutChar(uint16_t XPos, uint16_t YPos, char Ch, ST7789_FontTypeDef Font, ST7789_ColorTypeDef Color, ST7789_ColorTypeDef BackgroundColor)
{
	
	uint32_t heightCounter;
	uint32_t widthCounter;
	uint32_t fontByte;
	
	uint8_t colorBuff[2]   = {Color >> 8, Color & 0xFF};
	uint8_t bgColorBuff[2] = {BackgroundColor >> 8, BackgroundColor & 0xFF};
	
	/* --------------- Transmit Data ---------------- */
	ST7789_SetWindowAddress(XPos, YPos, XPos + Font.Width - 1, YPos + Font.Height - 1);

	for (heightCounter = 0; heightCounter < Font.Height; heightCounter++)
	{
		
		fontByte = Font.Data[(Ch - 32) * Font.Height + heightCounter];
		
		for (widthCounter = 0; widthCounter < Font.Width; widthCounter++)
		{
			
			if ((fontByte << widthCounter) & 0x8000)
			{
				ST7789_TransmitData(colorBuff, sizeof(colorBuff));
			}
			else
			{
				ST7789_TransmitData(bgColorBuff, sizeof(bgColorBuff));
			}
			
		}
		
	}
	
}

void ST7789_PutString(uint16_t XPos, uint16_t YPos, const char *Str, ST7789_FontTypeDef Font, ST7789_ColorTypeDef Color, ST7789_ColorTypeDef BackgroundColor)
{
	
	/* --------------- Put Characters --------------- */
	while (*Str)
	{
		
		if (XPos + Font.Width >= ST7789_WIDTH_MODIFIED)
		{
			
			XPos  = 0;
			YPos += Font.Height;
			
			if (YPos + Font.Height >= ST7789_HEIGHT_MODIFIED)
			{
				break;
			}
			
			/* Skip space in the beginning of the new line */
			if (*Str == ' ')
			{
				Str++;
				continue;
			}
			
		}
		
		ST7789_PutChar(XPos, YPos, *Str, Font, Color, BackgroundColor);
		
		XPos += Font.Width;
		Str++;
		
	}
	
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
