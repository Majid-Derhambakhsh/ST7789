/*
------------------------------------------------------------------------------
~ File   : st7789_font.h
~ Author : Majid Derhambakhsh
~ Version: V1.0.0
~ Created: 08/25/2023 18:00:00 PM
~ Brief  :
~ Support:
           E-Mail : Majid.Derhambakhsh@gmail.com (subject : Embedded Library Support)
           
           Github : https://github.com/Majid-Derhambakhsh
------------------------------------------------------------------------------
~ Description:    

~ Attention  :    
                  
~ Changes    :    
------------------------------------------------------------------------------
*/

#ifndef __ST7789_FONT_H_
#define __ST7789_FONT_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "st7789.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Fonts ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* .......... Font Type .......... */
extern ST7789_FontTypeDef Font_7x10;
extern ST7789_FontTypeDef Font_11x18;
extern ST7789_FontTypeDef Font_16x26;

/* .......... Font Data .......... */
extern const uint16_t Font7x10Data[];
extern const uint16_t Font11x18Data[];
extern const uint16_t Font16x26Data[];

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* __ST7789_FONT_H_ */
