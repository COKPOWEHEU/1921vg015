#ifndef __LCD_ILI9341_H__
#define __LCD_ILI9341_H__

#include "system_k1921vg015.h"
//LCD_orientation
#ifndef LCD_MIR_X
  #define LCD_MIR_X 0
#endif
#ifndef LCD_MIR_Y
  #define LCD_MIR_Y 1
#endif
#ifndef LCD_ORIENT_V
  #define LCD_ORIENT_V 1
#endif
#ifndef LCD_ML
  #define LCD_ML 0
#endif
#ifndef LCD_RGB
  #define LCD_RGB 1
#endif
#ifndef LCD_MH
  #define LCD_MH 0
#endif

#if LCD_ORIENT_V
  #define LCD_MAXX 239
  #define LCD_MAXY 319
#else
  #define LCD_MAXX 319
  #define LCD_MAXY 239
#endif

#define LCD_CS		B,7,0,GPIO_PP
#define LCD_RST		B,6,0,GPIO_PP
#define LCD_DC		B,5,0,GPIO_PP

#define SPI0_TX		B,3,1,GPIO_AF1
#define SPI0_RX		B,2,1,GPIO_AF1
#define SPI0_CS		B,1,0,GPIO_PP
#define SPI0_SCK	B,0,1,GPIO_AF1
#define SPI0_TX_DMA	17, do{SPI0->DMACR_bit.TXDMAE = 1;}while(0), do{SPI0->DMACR_bit.TXDMAE = 0;}while(0)
//#define SPI0_RX_DMA	20

//#define SPI1_TX		B,7,1,GPIO_AF1
//#define SPI1_RX		B,6,1,GPIO_AF1
//#define SPI1_CS		B,5,0,GPIO_PP
//#define SPI1_SCK	B,4,1,GPIO_AF1
//#define SPI1_TX_DMA	18
//#define SPI1_RX_DMA	21

void lcd_init();
void lcd_test();
void lcd_wait();
void lcd_begin_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_dma_init(uint8_t *buf, uint16_t size);
void lcd_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col);
void lcd_str(int16_t x, int16_t y, uint8_t sz, char *str);

#if LCD_RGB==1
  #define rgb2col(r,g,b) (((r<<8)&0b1111100000000000)|((g<<3)&0b0000011111100000)|(b>>3))
#else
  #define rgb2col(r,g,b) (((b<<8)&0b1111100000000000)|((g<<3)&0b0000011111100000)|(r>>3))
#endif

#endif