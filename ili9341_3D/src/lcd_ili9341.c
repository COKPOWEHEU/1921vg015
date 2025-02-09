#include "K1921VG015.h"
#include "lcd_ili9341.h"
#include "pinmacro.h"
#include "dma.h"
#include "clock.h"

void lcd_spi_init(){
  RCU->CGCFGAHB |= RCU_CGCFGAHB_SPI0EN_Msk;
  RCU->RSTDISAHB |= RCU_RSTDISAHB_SPI0EN_Msk;
  SPI0->CR1_bit.SSE = 0;
  SPI0->DMACR_bit.TXDMAE = 0;
  RCU->SPICLKCFG[0].SPICLKCFG = 
    //(RCU_SPICLKCFG_CLKSEL_HSI << RCU_SPICLKCFG_CLKSEL_Pos) |
    (RCU_SPICLKCFG_CLKSEL_PLL0 << RCU_SPICLKCFG_CLKSEL_Pos) |
    RCU_SPICLKCFG_CLKEN_Msk |
    RCU_SPICLKCFG_RSTDIS_Msk;
    
  
  SPI0->CPSR = 2; //только четные значения!
  SPI0->CR0 = 
    (0 << SPI_CR0_SCR_Pos) | //divider
    (0 << SPI_CR0_SPH_Pos) |
    (0 << SPI_CR0_SPO_Pos) |
    (0b00 << SPI_CR0_FRF_Pos) | //SPI subtype
    ((8-1) << SPI_CR0_DSS_Pos); //word size
    
  SPI0->CR1_bit.MS = 0; //master
  SPI0->CR1_bit.SSE = 1; //SPI enable
    
  GPIO_config(SPI0_TX);
  GPIO_config(SPI0_RX);
  GPIO_config(SPI0_SCK);
}

void spi_wait(){
  while(SPI0->SR & SPI_SR_BSY_Msk){}
}

void spi_send(uint8_t data){
  SPI0->DR = data;
  while(SPI0->SR & SPI_SR_BSY_Msk){}
}

void lcd_stop(){
  spi_wait();
  GPO_OFF(LCD_CS);
}

void lcd_send(uint8_t data){
  spi_wait();
  spi_send(data);
}

void lcd_start(uint8_t cmd){
  GPO_ON(LCD_DC);
  GPO_ON(LCD_CS);
  spi_send(cmd);
  GPO_OFF(LCD_DC);
}

void lcd_cmd(uint8_t cmd){
  lcd_start(cmd);
  lcd_stop();
}

void lcd_data(uint8_t data){
  spi_wait();
  GPO_OFF(LCD_DC);
  GPO_ON(LCD_CS);
  spi_send(data);
  GPO_OFF(LCD_CS);
}

void lcd_word(uint16_t data){
  lcd_send((uint8_t)(data>>8));
  lcd_send((uint8_t)data);
}

void lcd_init(){
  GPIO_config(LCD_CS);
  GPIO_config(LCD_DC);
  GPIO_config(LCD_RST);
  lcd_spi_init();
  
  GPO_ON(LCD_RST);
  delay_ticks(1000000);
  GPO_OFF(LCD_RST);
  delay_ticks(1000000);
  
  //soft reset
  lcd_cmd(0x01);
  delay_ticks(1000000);
  //power control A
  lcd_start(0xCB); lcd_send(0x39); lcd_send(0x2C); lcd_send(0x00);
                   lcd_send(0x34); lcd_send(0x02); lcd_stop();
  //power control B
  lcd_start(0xCF); lcd_send(0x00); lcd_send(0xC1); lcd_send(0x30); lcd_stop();
  //timing control A
  lcd_start(0xE8); lcd_send(0x85); lcd_send(0x00); lcd_send(0x78); lcd_stop();
  //timing control B
  lcd_start(0xEA); lcd_send(0x00); lcd_send(0x00); lcd_stop();
  //poweOn control
  lcd_start(0xED); lcd_send(0x64); lcd_send(0x03); lcd_send(0x12); lcd_send(0x81); lcd_stop();
  //display function control
  lcd_start(0xB6); lcd_send(0x08); lcd_send(0x82); lcd_send(0x27); lcd_stop();
  //gamma pos
  lcd_start(0xE0); lcd_send(0x0F); lcd_send(0x31); lcd_send(0x2B); lcd_send(0x0C);
                   lcd_send(0x0E); lcd_send(0x08); lcd_send(0x4E); lcd_send(0xF1);
                   lcd_send(0x37); lcd_send(0x07); lcd_send(0x10); lcd_send(0x03);
                   lcd_send(0x0E); lcd_send(0x09); lcd_send(0x00); lcd_stop();
  //gamma neg
  lcd_start(0xE1); lcd_send(0x00); lcd_send(0x0E); lcd_send(0x14); lcd_send(0x03);
                   lcd_send(0x11); lcd_send(0x07); lcd_send(0x31); lcd_send(0xC1);
                   lcd_send(0x48); lcd_send(0x08); lcd_send(0x0F); lcd_send(0x0C);
                   lcd_send(0x31); lcd_send(0x36); lcd_send(0x0F); lcd_stop();
  //pump ratio control
  lcd_cmd(0xF7); lcd_data(0x20);
  //power control 1
  lcd_cmd(0xC0); lcd_data(0x23);
  //power control 2
  lcd_cmd(0xC1); lcd_data(0x10);
  //VCOM control 1
  lcd_cmd(0xC5); lcd_data(0x3e); lcd_data(0x28);
  //VCOM control 2
  lcd_cmd(0xC7); lcd_data(0x86);
  //pixel format
  lcd_cmd(0x3A); lcd_data(0x55);
  //Frame control
  lcd_cmd(0xB1); lcd_data(0x00); lcd_data(0x18);
  //enable 3G
  lcd_cmd(0xF2); lcd_data(0x00);
  //gamma set
  lcd_cmd(0x26); lcd_data(0x01);
  //memory access control
  lcd_start(0x36);
  lcd_send((LCD_MIR_Y<<7)|(LCD_MIR_X<<6)|(LCD_ORIENT_V<<5)|(LCD_ML<<4)|(LCD_RGB<<3)|(LCD_MH<<2));
  lcd_stop();
  
  //sleep out
  lcd_cmd(0x11);
  delay_ticks(1000000);
  //display on
  lcd_cmd(0x29);
  //lcd_stop();
  
  lcd_stop();
}

void lcd_test(){
  uint32_t i, w=LCD_MAXX+1, h=LCD_MAXY+1, n;
  n = w*h;
  
  spi_wait();
  lcd_cmd(0x00);
  lcd_start(0x2A); lcd_word(0); lcd_word(h); lcd_stop();
  lcd_start(0x2B); lcd_word(0); lcd_word(w); lcd_stop();
  lcd_start(0x2C);
  
  for(i=0;i<n;i++){
    lcd_word(i + __TIME_S__);
  }
  
  lcd_stop();
}

void lcd_begin_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
  lcd_wait();
  lcd_cmd(0x00);
  lcd_start(0x2A); lcd_word(y1); lcd_word(y2); lcd_stop();
  lcd_start(0x2B); lcd_word(x1); lcd_word(x2); lcd_stop();
  lcd_start(0x2C);
}

void lcd_wait(){
  spi_wait();
}

void lcd_dma_init(uint8_t *buf, uint16_t size){
  dma_cfg_io(SPI0_TX_DMA, &SPI0->DR, buf, size*2);
  dma_cfg_mem(SPI0_TX_DMA, 8,0, 8,1, 0, 0);
  dma_enable(SPI0_TX_DMA);
}

void lcd_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col){
  lcd_begin_area(x1, y1, x2, y2);
  uint32_t n = (x2-x1)*(y2-y1);
  for(uint32_t i=0; i<n; i++)lcd_word(col);
  lcd_stop();
}

#define LCD_CHARS_FULL_TABLE

#include "lcd_chars.h"

uint16_t cur_x=0, cur_y=0;
uint8_t lcd_font_size = 1;
struct{
  uint16_t fg;
  uint16_t bg;
}lcd_color = {
  .bg = 0,
  .fg = 0xFFFF
};

void lcd_goto(uint16_t x, uint16_t y){
  cur_x = x * CHAR_W * lcd_font_size;
  cur_y = y * CHAR_H * lcd_font_size;
}

void lcd_char(char ch){
  char temp;
  uint16_t fs = lcd_font_size;
  uint16_t i, j;
  uint16_t x0, y0, x2, y2;

  if(ch>=CHAR_NUM)ch=0;
  x2 = cur_x + (CHAR_W+1)*fs;
  y2 = cur_y+CHAR_H*fs;
  if(x2 > LCD_MAXX){
    cur_x = 0; x2 = cur_x + (CHAR_W+1)*fs;
    cur_y = y2; y2 = cur_y+CHAR_H*fs;
  }
  if(y2 > LCD_MAXY+1)return;

  lcd_begin_area(cur_x,cur_y, x2,y2-1);

  uint16_t clr;
  for(x0=0; x0<CHAR_H*fs*fs; x0++)lcd_word(lcd_color.bg);
    
  for(y0=0; y0<CHAR_W; y0++){
    for(i=0; i<fs; i++){
      temp = char_table[(unsigned char)ch][y0];
      for(x0=0; x0<CHAR_H; x0++){
        if(temp & (1<<0))clr = lcd_color.fg; else clr = lcd_color.bg;
        for(j=0; j<fs; j++)lcd_word(clr);
        temp>>=1;
      }
    }
  }
  //lcd_wait();
  //while(!spi_ready()){}

  //lcd_end_area();
  lcd_stop();
  cur_x = x2;
}

void lcd_str(int16_t x, int16_t y, uint8_t sz, char *str){
  if(x >= 0)cur_x = x;
  if(y >= 0)cur_y = y;
  lcd_font_size = sz;
  while(str[0]){
    lcd_char(str[0]);
    str++;
  }
}