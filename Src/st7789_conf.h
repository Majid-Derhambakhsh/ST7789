/*
------------------------------------------------------------------------------
~ File   : st7789_conf.h
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

#ifndef __ST7789_CFG_H_
#define __ST7789_CFG_H_

/* ~~~~~~~~~~~~~~ Required Headers ~~~~~~~~~~~~~ */
/* Driver-library for AVR */
//#include "gpio_unit.h"
//#include "spi_unit.h"

/* Driver-library for STM32 */
#include "main.h"
#include "spi.h"

/* ~~~~~~~~~~~~~~~~~~~ Fonts ~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~ Configurations ~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~ SPI ~~~~~~~~~~~ */
#define ST7789_SPI             hspi2

#define ST7789_RST_GPIO_PORT   LCD_RST_GPIO_Port
#define ST7789_RST_GPIO_PIN    LCD_RST_Pin

#define ST7789_CS_GPIO_PORT    LCD_CS_GPIO_Port
#define ST7789_CS_GPIO_PIN     LCD_CS_Pin

#define ST7789_DC_GPIO_PORT    LCD_DC_GPIO_Port
#define ST7789_DC_GPIO_PIN     LCD_DC_Pin

#define ST7789_SPI_TIMEOUT     1000

/*
	Guide :
			ST7789_SPI              : Specifies the SPI peripheral
			
			ST7789_RST_GPIO_PORT    : Specifies the Reset port
			ST7789_RST_GPIO_PIN     : Specifies the Reset pin
			
			ST7789_CS_GPIO_PORT     : Specifies the CS (CSN) port
			ST7789_CS_GPIO_PIN      : Specifies the CS (CSN) pin
			
			ST7789_DC_GPIO_PORT     : Specifies the DC port
			ST7789_DC_GPIO_PIN      : Specifies the DC pin
	
	Example:
			ARM:
				ST7789_SPI            hspi1
				
				ST7789_RST_GPIO_PORT  GPIOA
				ST7789_RST_GPIO_PIN   GPIO_PIN_0
				
				ST7789_CS_GPIO_PORT   GPIOA
				ST7789_CS_GPIO_PIN    GPIO_PIN_1
				
				ST7789_DC_GPIO_PORT   GPIOA
				ST7789_DC_GPIO_PIN    GPIO_PIN_2
				
				ST7789_SPI_TIMEOUT     1000
*/

/* ~~~~~~~~~~ DMA ~~~~~~~~~~~ */
// Comment the line if don't use DMA
#define ST7789_USE_DMA

#define ST7789_DMA_MIN_SIZE  16 // Minimum size of data length

/* ~~~~~~~~~ Buffer ~~~~~~~~~ */
/*

	The LCD Buffer length is determined as follows:
	
		LCD_BUFFER_LENGTH = ST7789_WIDTH * ST7789_HOR_LEN
	
	Notice:
		- DMA needs the LCD Buffer to transmit packets
		- If your MCU RAM size is very low, disable DMA
		
		- Increase LCD Buffer length to LCD resolution size for increased speed (If MCU has enough RAM)
	
*/

#define ST7789_HOR_LEN 	     5 // Buffer length factor

/* ~~~~~~~~~~~~~~~~~~ Options ~~~~~~~~~~~~~~~~~~ */
#define ST7789_WIDTH     240
#define ST7789_HEIGHT    280
#define ST7789_ROTATION  2

/* ~~~~~~~~ STM32 MCU ~~~~~~~ */
#define STM32F1

/*
	Guide   :
				#define STM32Xx : STM32 Family.
	Example :
				#define STM32F1
				#define STM32H7
				#define STM32L0
*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* __ST7789_CFG_H_ */
