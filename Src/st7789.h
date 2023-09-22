/*
------------------------------------------------------------------------------
~ File   : st7789.h
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

#ifndef __ST7789_H_
#define __ST7789_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <math.h>
#include <stdint.h>

#include "st7789_conf.h"

#ifdef ST7789_USE_DMA
	#include <string.h>
#endif

/* ------------------------------------------------------------------ */
#ifdef _CodeVISIONAVR__  /* Check compiler */

#pragma warn_unref_func- /* Disable 'unused function' warning */

#ifndef __SPI_UNIT_H_
	#include "SPI_UNIT/spi_unit.h"
#endif /* __SPI_UNIT_H_ */

#ifndef __GPIO_UNIT_H_
	#include "GPIO_UNIT/gpio_unit.h"
#endif /* __GPIO_UNIT_H_ */

/* ------------------------------------------------------------------ */
#elif defined(__GNUC__) && !defined(USE_HAL_DRIVER)  /* Check compiler */

#pragma GCC diagnostic ignored "-Wunused-function" /* Disable 'unused function' warning */

#ifndef __SPI_UNIT_H_
	#include "SPI_UNIT/spi_unit.h"
#endif /* __SPI_UNIT_H_ */

#ifndef __GPIO_UNIT_H_
	#include "GPIO_UNIT/gpio_unit.h"
#endif /* __GPIO_UNIT_H_ */

/* ------------------------------------------------------------------ */
#elif defined(USE_HAL_DRIVER)  /* Check driver */

	#if LCD_USE_FREE_RTOS == 1
		#include "cmsis_os.h"
	#endif /* _LCD_USE_FREE_RTOS */
	
	/* --------------- Check Mainstream series --------------- */
	#ifdef STM32F0
		#include "stm32f0xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F1)
		#include "stm32f1xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F2)
		#include "stm32f2xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F3)
		#include "stm32f3xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F4)
		#include "stm32f4xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F7)
		#include "stm32f7xx_hal.h"       /* Import HAL library */
	#elif defined(STM32G0)
		#include "stm32g0xx_hal.h"       /* Import HAL library */
	#elif defined(STM32G4)
		#include "stm32g4xx_hal.h"       /* Import HAL library */
	
	/* ------------ Check High Performance series ------------ */
	#elif defined(STM32H7)
		#include "stm32h7xx_hal.h"       /* Import HAL library */
	
	/* ------------ Check Ultra low power series ------------- */
	#elif defined(STM32L0)
		#include "stm32l0xx_hal.h"       /* Import HAL library */
	#elif defined(STM32L1)
		#include "stm32l1xx_hal.h"       /* Import HAL library */
	#elif defined(STM32L5)
		#include "stm32l5xx_hal.h"       /* Import HAL library */
	#elif defined(STM32L4)
		#include "stm32l4xx_hal.h"       /* Import HAL library */
	#elif defined(STM32H7)
		#include "stm32h7xx_hal.h"       /* Import HAL library */
	#else
	#endif /* STM32F1 */
	
	/* ------------------------------------------------------- */
	#if defined(__ICCARM__) /* ICCARM Compiler */
	
		#pragma diag_suppress=Pe177   /* Disable 'unused function' warning */
		
	#elif defined(__GNUC__) /* GNU Compiler */
	
		//#pragma diag_suppress 177     /* Disable 'unused function' warning */
		
	#endif /* __ICCARM__ */

/* ------------------------------------------------------------------ */
#else                     /* Compiler not found */

#error Chip or Library not supported  /* Send error */

#endif /* _CodeVISIONAVR__ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ----------------------------- ST7789 ---------------------------- */
#define ST7789_WIDTH_MAX   240
#define ST7789_HEIGHT_MAX  320

/* Frame Memory Position Algorithm:
	
	Without Rotation:
		X Offset: (ST7789_WIDTH_MAX - ST7789_WIDTH) / 2
		Y Offset: (ST7789_HEIGHT_MAX - ST7789_HEIGHT) / 2
		
	With Rotation:
		X Offset: (ST7789_HEIGHT_MAX - ST7789_WIDTH) / 2
		Y Offset: (ST7789_WIDTH_MAX - ST7789_HEIGHT) / 2
			
*/
#if ((ST7789_ROTATION == 0) || (ST7789_ROTATION == 2))

	#define ST7789_WIDTH_MODIFIED   ST7789_WIDTH
	#define ST7789_HEIGHT_MODIFIED  ST7789_HEIGHT
	
	#define ST7789_XS  ((ST7789_WIDTH_MAX - ST7789_WIDTH_MODIFIED) / 2)
	#define ST7789_YS  ((ST7789_HEIGHT_MAX - ST7789_HEIGHT_MODIFIED) / 2)
	
#else
	
	#define ST7789_WIDTH_MODIFIED   ST7789_HEIGHT
	#define ST7789_HEIGHT_MODIFIED  ST7789_WIDTH
	
	#define ST7789_XS  ((ST7789_HEIGHT_MAX - ST7789_WIDTH_MODIFIED) / 2)
	#define ST7789_YS  ((ST7789_WIDTH_MAX - ST7789_HEIGHT_MODIFIED) / 2)
	
#endif

/* .................... Common .................... */
/* ................... Macro's .................... */

/* ----------------------- Define by compiler ---------------------- */
#if (defined(_CodeVISIONAVR__) || defined(__GNUC__)) && !defined(USE_HAL_DRIVER)
	
	#undef ST7789_USE_DMA
	
	#ifndef ST7789_GPIO_WritePin
		#define ST7789_GPIO_WritePin(gpiox , gpio_pin , pin_state)           GPIO_WritePin(&(gpiox) , (uint8_t)(gpio_pin) , pin_state)
	#endif
	
	#ifndef ST7789_GPIO_PIN_SET
		#define ST7789_GPIO_PIN_SET                                          _GPIO_PIN_SET
	#endif
	
	#ifndef ST7789_GPIO_PIN_RESET
		#define ST7789_GPIO_PIN_RESET                                        _GPIO_PIN_RESET
	#endif
	
	#define ST7789_Delay(t)                                                  _delay_ms((t))
	
	#ifndef ST7789_SPI_TransmitReceive
		#define ST7789_SPI_TransmitReceive(pTxData, pRxData, Size, Timeout)  SPI_TransmitReceive((pTxData), (pRxData), (Size), (Timeout))
		#define ST7789_SPI_Transmit(pTxData, Size, Timeout)                  SPI_Transmit((pTxData), (Size), (Timeout))
		#define ST7789_SPI_Receive(pRxData, Size, Timeout)                   SPI_Receive((pRxData), (Size), (Timeout))
	#endif
	
/*----------------------------------------------------------*/
#elif defined(USE_HAL_DRIVER)
	
	#ifndef ST7789_GPIO_WritePin
		#define ST7789_GPIO_WritePin(gpiox , gpio_pin , pin_state)           HAL_GPIO_WritePin((gpiox) , (uint32_t)(gpio_pin) , (pin_state))
	#endif
	
	#ifndef ST7789_GPIO_PIN_SET
		#define ST7789_GPIO_PIN_SET                                          GPIO_PIN_SET
	#endif
	
	#ifndef ST7789_GPIO_PIN_RESET
		#define ST7789_GPIO_PIN_RESET                                        GPIO_PIN_RESET
	#endif
	
	#ifndef ST7789_DMA_STATE_READY
		#define ST7789_DMA_STATE_READY                                       HAL_DMA_STATE_READY
	#endif
	
	#ifndef ST7789_SPI_STATE_READY
		#define ST7789_SPI_STATE_READY                                       HAL_SPI_STATE_READY
	#endif
	
	#define ST7789_Delay(t)                                                  HAL_Delay((t))
	
	#ifndef ST7789_SPI_TransmitReceive
		#define ST7789_SPI_TransmitReceive(pTxData, pRxData, Size, Timeout)  HAL_SPI_TransmitReceive(&ST7789_SPI, (pTxData), (pRxData), (Size), (Timeout))
		#define ST7789_SPI_Transmit(pTxData, Size, Timeout)                  HAL_SPI_Transmit(&ST7789_SPI, (pTxData), (Size), (Timeout))
		#define ST7789_SPI_Transmit_DMA(pTxData, Size)                       HAL_SPI_Transmit_DMA(&ST7789_SPI, (pTxData), (Size))
		#define ST7789_SPI_Receive(pRxData, Size, Timeout)                   HAL_SPI_Receive(&ST7789_SPI, (pRxData), (Size), (Timeout))
	#endif

#endif /* __GNUC__ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ------------------------ ST7789 Commands ------------------------ */
typedef enum /* LCD Commands */
{
	
	/* SYSTEM FUNCTION COMMAND TABLE 1 */
	ST7789_CMD_NOP     = 0x00,
	ST7789_CMD_SWRESET = 0x01, // Software Reset
	ST7789_CMD_RDDID   = 0x04, // Read Display ID
	ST7789_CMD_RDDST   = 0x09, // Read Display Status
	
	ST7789_CMD_SLPIN   = 0x10, // Sleep in
	ST7789_CMD_SLPOUT  = 0x11, // Sleep Out
	ST7789_CMD_PTLON   = 0x12, // Partial Display Mode On
	ST7789_CMD_NORON   = 0x13, // Normal Display Mode On
	
	ST7789_CMD_INVOFF  = 0x20, // Display Inversion Off
	ST7789_CMD_INVON   = 0x21, // Display Inversion On
	ST7789_CMD_DISPOFF = 0x28, // Display Off
	ST7789_CMD_DISPON  = 0x29, // Display On
	ST7789_CMD_CASET   = 0x2A, // Column Address Set
	ST7789_CMD_RASET   = 0x2B, // Row Address Set
	ST7789_CMD_RAMWR   = 0x2C, // Memory Write
	ST7789_CMD_RAMRD   = 0x2E, // Memory Read
	
	ST7789_CMD_PTLAR   = 0x30, // Partial Area
	ST7789_CMD_COLMOD  = 0x3A, // Interface Pixel Format
	ST7789_CMD_MADCTL  = 0x36, // Memory Data Access Control
	
	ST7789_CMD_WRMEMC  = 0x3C, // Write Memory Continue
	
	ST7789_CMD_RDID1   = 0xDA, // Read ID1
	ST7789_CMD_RDID2   = 0xDB, // Read ID2
	ST7789_CMD_RDID3   = 0xDC, // Read ID3
	ST7789_CMD_RDID4   = 0xDD, // Read ID4
	
	/* SYSTEM FUNCTION COMMAND TABLE 2 */
	ST7789_CMD_PORCTRL   = 0xB2, //  Porch Setting
	ST7789_CMD_GCTRL     = 0xB7, // Gate Control
	ST7789_CMD_VCOMS     = 0xBB, // VCOM Setting
	ST7789_CMD_LCMCTRL   = 0xC0, // LCM Control
	ST7789_CMD_VDVVRHEN  = 0xC2, // VDV and VRH Command Enable
	ST7789_CMD_VRHS      = 0xC3, // VRH Set
	ST7789_CMD_VDVS      = 0xC4, // VDV Set
	ST7789_CMD_FRCTRL2   = 0xC6, // Frame Rate Control in Normal Mode
	ST7789_CMD_PWCTRL1   = 0xD0, // Power Control 1
	ST7789_CMD_PVGAMCTRL = 0xE0, // Positive Voltage Gamma Control
	ST7789_CMD_NVGAMCTRL = 0xE1, // Negative Voltage Gamma Control
	
}ST7789_CMDTypeDef;

/* ----------------------- ST7789 Register's ----------------------- */
typedef enum /* ST7789+ Default Registers Value */
{
	
	ST7789_REG_DEF_GCTRL      = 0x35,
	ST7789_REG_DEF_VCOMS      = 0x19,
	ST7789_REG_DEF_LCMCTRL    = 0x2C,
	ST7789_REG_DEF_VDVVRHEN   = 0x01,
	ST7789_REG_DEF_VRHS       = 0x12,
	ST7789_REG_DEF_VDVS       = 0x20,
	ST7789_REG_DEF_FRCTRL2    = 0x0F, // 60Hz Frame Rate
	ST7789_REG_DEF_PWCTRL1H   = 0xA4,
	ST7789_REG_DEF_PWCTRL1L   = 0xA1,
	
	/*
		Memory Data Access Control Register, ST7789_CMD_MADCTL (0x36H)
	
		Bit  : D7   D6   D5   D4   D3   D2   D1   D0
		Param: MY   MX   MV   ML   RGB  MH   -    -
	*/
	ST7789_REG_DEF_MADCTL_MY  = 0x80, // Page Address Order ('0': Top to Bottom, '1': The Opposite)
	ST7789_REG_DEF_MADCTL_MX  = 0x40, // Column Address Order ('0': Left to Right, '1': The Opposite)
	ST7789_REG_DEF_MADCTL_MV  = 0x20, // Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode)
	ST7789_REG_DEF_MADCTL_ML  = 0x10, // Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = The Opposite)
	ST7789_REG_DEF_MADCTL_RGB = 0x00, // RGB/BGR Order ('0' = RGB, '1' = BGR)
	
}ST7789_RegRSTValTypeDef;

/* ---------------------- ST7789 Parameter's ----------------------- */
typedef enum /* LCD Colors */
{
	
	ST7789_COLOR_WHITE       = 0xFFFF,
	ST7789_COLOR_BLACK       = 0x0000,
	ST7789_COLOR_BLUE        = 0x001F,
	ST7789_COLOR_RED         = 0xF800,
	ST7789_COLOR_MAGENTA     = 0xF81F,
	ST7789_COLOR_GREEN       = 0x07E0,
	ST7789_COLOR_CYAN        = 0x7FFF,
	ST7789_COLOR_YELLOW      = 0xFFE0,
	ST7789_COLOR_GRAY        = 0x8430,
	ST7789_COLOR_BRED        = 0xF81F,
	ST7789_COLOR_GRED        = 0xFFE0,
	ST7789_COLOR_GBLUE       = 0x07FF,
	ST7789_COLOR_BROWN       = 0xBC40,
	ST7789_COLOR_BRRED       = 0xFC07,
	ST7789_COLOR_DARKBLUE    = 0x01CF,
	ST7789_COLOR_LIGHTBLUE   = 0x7D7C,
	ST7789_COLOR_GRAYBLUE    = 0x5458,

	ST7789_COLOR_LIGHTGREEN  = 0x841F,
	ST7789_COLOR_LGRAY       = 0xC618,
	ST7789_COLOR_LGRAYBLUE   = 0xA651,
	ST7789_COLOR_LBBLUE      = 0x2B12,
	
}ST7789_ColorTypeDef;

typedef enum /* Color Invert */
{
	
	ST7789_INVERT_OFF = 0x20,
	ST7789_INVERT_ON  = 0x21,
	
}ST7789_InvTypeDef;

typedef enum /* Tear */
{
	
	ST7789_TEAR_OFF = 0x34,
	ST7789_TEAR_ON  = 0x35,
	
}ST7789_TearTypeDef;

typedef enum /* Color Mode */
{
	
	ST7789_COLOR_MODE_16BIT = 0x55, //  RGB565 (16bit)
	ST7789_COLOR_MODE_18BIT = 0x66, //  RGB666 (18bit)
	
}ST7789_ColorModeTypeDef;

typedef struct /* ST7789 Font */
{
	
	const uint8_t  Width;
	const uint8_t  Height;
	
	const uint16_t *Data;
	
}ST7789_FontTypeDef;

/* ---------------------------- Common ----------------------------- */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ......................... Initialize ........................ */
/*
 * Function: ST7789_Init
 * ---------------------
 * Initialize ST7789 LCD
 *
 * Param  : 
 *         -
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         
 *         ST7789_Init();
 *         ...
 *         
 */
void ST7789_Init(void);

/* .......................... Control .......................... */
/*
 * Function: ST7789_SetRotation
 * ----------------------------
 * Set display rotation
 *
 * Param  : 
 *         Rotation : Display rotation (0, 1, 2 or 3)
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_SetRotation(1);
 *         ...
 *         
 */
void ST7789_SetRotation(uint8_t Rotation);

/*
 * Function: ST7789_SetWindowAddress
 * ---------------------------------
 * Set display window address to write data
 *
 * Param  : 
 *         XStart : Start position of the window in the x-axis
 *         YStart : Start position of the window in the y-axis
 *         XEnd   : The end position of the window in the x-axis
 *         YEnd   : The end position of the window in the y-axis
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_SetWindowAddress(0, 0, 200, 100);
 *         ...
 *         
 */
void ST7789_SetWindowAddress(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd);

/*
 * Function: ST7789_InvertColors
 * -----------------------------
 * Invert display colors
 *
 * Param  : 
 *         Invert : The state of color inverting (ST7789_INVERT_OFF / ST7789_INVERT_ON)
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_InvertColors(ST7789_INVERT_OFF);
 *         ...
 *         
 */
void ST7789_InvertColors(ST7789_InvTypeDef Invert);

/*
 * Function: ST7789_TearEffect
 * ---------------------------
 * Enable or disable the tear effect in LCD
 *
 * Param  : 
 *         Tear : The state of the tear effect (ST7789_TEAR_OFF / ST7789_TEAR_ON)
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_TearEffect(ST7789_TEAR_OFF);
 *         ...
 *         
 */
void ST7789_TearEffect(ST7789_ColorModeTypeDef Tear);

/* ...................... Color Converting ..................... */
/*
 * Function: ST7789_Color_GetFromRGB
 * ---------------------------------
 * Convert RGB color format to RGB565 format
 *
 * Param  : 
 *         R : Red value
 *         G : Green value
 *         B : Blue value
 *         
 * Returns: 
 *         RGB565 Code : Converted RGB value
 *         
 * Example: 
 *         ST7789_ColorTypeDef color = ST7789_Color_GetFromRGB(252, 3, 140);
 *         ...
 *         
 */
ST7789_ColorTypeDef ST7789_Color_GetFromRGB(uint8_t R, uint8_t G, uint8_t B);

/*
 * Function: ST7789_Color_GetFromHex
 * ---------------------------------
 * Convert Hex color code to RGB565 format
 *
 * Param  : 
 *         HexCode : Hex color code
 *         
 * Returns: 
 *         RGB565 Code : Converted RGB value
 *         
 * Example: 
 *         ST7789_ColorTypeDef color = ST7789_Color_GetFromHex(0xFC038C);
 *         ...
 *         
 */
ST7789_ColorTypeDef ST7789_Color_GetFromHex(uint32_t HexCode);

/* ........................... Fill ............................ */
/*
 * Function: ST7789_Fill
 * ---------------------
 * Fill part of the display with the desired color
 *
 * Param  : 
 *         XStart : Start position in the x-axis
 *         YStart : Start position in the y-axis
 *         XEnd   : The end position in the x-axis
 *         YEnd   : The end position in the y-axis
 *         Color  : Desired color to fill the screen
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_Fill(0, 0, 50, 50, ST7789_COLOR_GREEN); // Fill with predefined color
 *         ST7789_Fill(70, 70, 150, 100, ST7789_Color_GetFromHex(0xFC038C)); // Fill with my custom color
 *         ...
 *         
 */
void ST7789_Fill(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_FillScreen
 * ---------------------------
 * Fill the display with the desired color
 *
 * Param  : 
 *         Color : Desired color to fill the screen
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_FillScreen(0, 0, 50, 50, ST7789_COLOR_WHITE);
 *         ...
 *         
 */
void ST7789_FillScreen(ST7789_ColorTypeDef Color);

/* .......................... Drawing .......................... */
/*
 * Function: ST7789_DrawPixel
 * --------------------------
 * Fill one pixel of the display with the desired color
 *
 * Param  : 
 *         XPos  : X position of pixel
 *         YPos  : Y position of pixel
 *         Color : Desired color to fill the pixel
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawPixel(10, 10, ST7789_COLOR_RED);
 *         ...
 *         
 */
void ST7789_DrawPixel(uint16_t XPos, uint16_t YPos, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawPixel_4px
 * ------------------------------
 * Fill one pixel of the display with 4px size and the desired color
 *
 * Param  : 
 *         XPos  : X position of pixel
 *         YPos  : Y position of pixel
 *         Color : Desired color to fill the pixel
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawPixel_4px(10, 10, ST7789_COLOR_RED);
 *         ...
 *         
 */
void ST7789_DrawPixel_4px(uint16_t XPos, uint16_t YPos, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawLine
 * -------------------------
 * Draw a line on the display
 *
 * Param  : 
 *         XStart : Start position of the line in the x-axis
 *         YStart : Start position of the line in the y-axis
 *         XEnd   : The end position of the line in the x-axis
 *         YEnd   : The end position of the line in the y-axis
 *         Color  : Desired color to draw line
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawLine(10, 10, 50, 100, ST7789_COLOR_RED);
 *         ...
 *         
 */
void ST7789_DrawLine(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawRectangle
 * ------------------------------
 * Draw a rectangle on the display
 *
 * Param  : 
 *         XStart : Start position of the rectangle in the x-axis
 *         YStart : Start position of the rectangle in the y-axis
 *         XEnd   : The end position of the rectangle in the x-axis
 *         YEnd   : The end position of the rectangle in the y-axis
 *         Color  : Desired color to draw rectangle
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawRectangle(10, 10, 50, 50, ST7789_COLOR_GREEN);
 *         ...
 *         
 */
void ST7789_DrawRectangle(uint16_t XStart, uint16_t YStart, uint16_t XEnd, uint16_t YEnd, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawFilledRectangle
 * ------------------------------------
 * Draw a filled rectangle on the display
 *
 * Param  : 
 *         XPos   : X position of the rectangle
 *         YPos   : Y position of the rectangle
 *         Width  : Width of the rectangle
 *         Height : Height of the rectangle
 *         Color  : Desired color to draw rectangle
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawFilledRectangle(10, 10, 50, 50, ST7789_COLOR_GREEN);
 *         ...
 *         
 */
void ST7789_DrawFilledRectangle(uint16_t XPos, uint16_t YPos, uint16_t Width, uint16_t Height, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawCircle
 * ---------------------------
 * Draw a circle on the display
 *
 * Param  : 
 *         XPos   : X position of the circle
 *         YPos   : Y position of the circle
 *         Radius : The radius of the circle
 *         Color  : Desired color to draw circle
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawCircle(100, 100, 50, ST7789_COLOR_GREEN);
 *         ...
 *         
 */
void ST7789_DrawCircle(uint16_t XPos, uint16_t YPos, uint8_t Radius, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawFilledCircle
 * ---------------------------------
 * Draw a filled circle on the display
 *
 * Param  : 
 *         XPos   : X position of the circle
 *         YPos   : Y position of the circle
 *         Radius : The radius of the circle
 *         Color  : Desired color to draw circle
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawFilledCircle(100, 100, 50, ST7789_COLOR_GREEN);
 *         ...
 *         
 */
void ST7789_DrawFilledCircle(int16_t XPos, int16_t YPos, int16_t Radius, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawTriangle
 * -----------------------------
 * Draw a triangle on the display
 *
 * Param  : 
 *         X1    : X position of the first side of the triangle
 *         Y1    : Y position of the first side of the triangle
 *         X2    : X position of the second side of the triangle
 *         Y2    : Y position of the second side of the triangle
 *         X3    : X position of the third side of the triangle
 *         Y3    : Y position of the third side of the triangle
 *         Color : Desired color to draw circle
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawTriangle(0, 0, 50, 50, 100, 50, ST7789_COLOR_GREEN);
 *         ...
 *         
 */
void ST7789_DrawTriangle(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t X3, uint16_t Y3, ST7789_ColorTypeDef Color);

/*
 * Function: ST7789_DrawFilledTriangle
 * -----------------------------------
 * Draw a filled triangle on the display
 *
 * Param  : 
 *         X1    : X position of the first side of the triangle
 *         Y1    : Y position of the first side of the triangle
 *         X2    : X position of the second side of the triangle
 *         Y2    : Y position of the second side of the triangle
 *         X3    : X position of the third side of the triangle
 *         Y3    : Y position of the third side of the triangle
 *         Color : Desired color to draw circle
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         ST7789_DrawFilledTriangle(0, 0, 50, 50, 100, 50, ST7789_COLOR_GREEN);
 *         ...
 *         
 */
void ST7789_DrawFilledTriangle(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t X3, uint16_t Y3, ST7789_ColorTypeDef Color);

/* ........................... Image ........................... */
/*
 * Function: ST7789_PutImage
 * -------------------------
 * Show picture on the display
 *
 * Param  : 
 *         XPos   : X position of the picture
 *         YPos   : Y position of the picture
 *         Width  : Width of the picture
 *         Height : Height of the picture
 *         Image  : Data of picture in RGB565 format
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         const uint8_t myLogo[] = {.....};
 *         
 *         ST7789_PutImage(100, 100, 140, 50, myLogo);
 *         ...
 *         
 */
void ST7789_PutImage(uint16_t XPos, uint16_t YPos, uint16_t Width, uint16_t Height, const uint16_t *Image);

/* ........................... Text ............................ */
/*
 * Function: ST7789_PutChar
 * ------------------------
 * Show character on the display
 *
 * Param  : 
 *         XPos            : X position of the character
 *         YPos            : Y position of the character
 *         Ch              : Ascii character
 *         Font            : Font of character
 *         Color           : Color of character
 *         BackgroundColor : Background color of character
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         const uint16_t Font7x10Data[] = {.....}; // Font data's
 *         
 *         ST7789_FontTypeDef Font_7x10  = {7 , 10, Font7x10Data}; // Define the font
 *         
 *         ST7789_PutChar(0, 0, 'A', Font_7x10, ST7789_COLOR_BLACK, ST7789_COLOR_WHITE);
 *         ...
 *         
 */
void ST7789_PutChar(uint16_t XPos, uint16_t YPos, char Ch, ST7789_FontTypeDef Font, ST7789_ColorTypeDef Color, ST7789_ColorTypeDef BackgroundColor);

/*
 * Function: ST7789_PutString
 * --------------------------
 * Show text on the display
 *
 * Param  : 
 *         XPos            : X position of the text
 *         YPos            : Y position of the text
 *         Str             : Text in string format
 *         Font            : Font of text
 *         Color           : Color of text
 *         BackgroundColor : Background color of text
 *         
 * Returns: 
 *         -
 *         
 * Example: 
 *         const uint16_t Font7x10Data[] = {.....}; // Font data's
 *         
 *         ST7789_FontTypeDef Font_7x10  = {7 , 10, Font7x10Data}; // Define the font
 *         
 *         ST7789_PutString(0, 0, 'Hello', Font_7x10, ST7789_COLOR_BLACK, ST7789_COLOR_WHITE);
 *         ...
 *         
 */
void ST7789_PutString(uint16_t XPos, uint16_t YPos, const char *Str, ST7789_FontTypeDef Font, ST7789_ColorTypeDef Color, ST7789_ColorTypeDef BackgroundColor);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* __ST7789_H_ */
