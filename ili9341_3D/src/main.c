#include "K1921VG015.h"
#include "system_k1921vg015.h"
#include "plic.h"
#include "pinmacro.h"
#define USART 0
#include "clock.h" 
#include "uart.h"
#include "strlib.h"
#include "dma.h"
#include "lcd_ili9341.h"
#include "gl.h"
#include "../model/fly.h"
#include "../model/lur.h"
#include "../model/shadow.h"

DMA_init_macro();

#define YLED	A,2,1,GPIO_PP
#define GLED1	A,12,1,GPIO_PP
#define RLED1	A,13,1,GPIO_PP
#define GLED2	A,14,1,GPIO_PP
#define RLED2	A,15,1,GPIO_PP
#define BTN		A,11,1,GPIO_HIZ


const struct glVector3 v[]={
  {.x=-1,.y=-1,.z=0,.col=rgb2col(0xFF,0,0)},
  {.x=1,.y=-1,.z=0,.col=rgb2col(0xFF,0,0)},
  {.x=-1,.y=1,.z=0,.col=rgb2col(0xFF,0,0)},
  {.x=1,.y=1,.z=0,.col=rgb2col(0xFF,0,0)},
  
  {.x=-1,.y=0,.z=-1,.col=rgb2col(0,0xFF,0)},
  {.x=1,.y=0,.z=-1,.col=rgb2col(0,0xFF,0)},
  {.x=-1,.y=0,.z=1,.col=rgb2col(0,0xFF,0)},
  {.x=1,.y=0,.z=1,.col=rgb2col(0,0xFF,0)},
  
  {.x=0,.y=-1,.z=-1,.col=rgb2col(0,0,0xFF)},
  {.x=0,.y=1,.z=-1,.col=rgb2col(0,0,0xFF)},
  {.x=0,.y=-1,.z=1,.col=rgb2col(0,0,0xFF)},
  {.x=0,.y=1,.z=1,.col=rgb2col(0,0,0xFF)}
};

int main(){
  RCU->CGCFGAHB |= RCU_CGCFGAHB_GPIOAEN_Msk | RCU_CGCFGAHB_GPIOBEN_Msk | RCU_CGCFGAHB_GPIOCEN_Msk;
  RCU->RSTDISAHB |= RCU_RSTDISAHB_GPIOAEN_Msk | RCU_RSTDISAHB_GPIOBEN_Msk | RCU_RSTDISAHB_GPIOCEN_Msk;
  GPIOA->ALTFUNCSET = 0; GPIOB->ALTFUNCSET = 0; GPIOC->ALTFUNCSET = 0; 
  GPIOA->OUTENCLR = 0; GPIOB->OUTENCLR = 0; GPIOC->OUTENCLR = 0;
  GPIOA->ALTFUNCNUM = 0; GPIOB->ALTFUNCNUM = 0; GPIOC->ALTFUNCNUM = 0;
  dma_init();
  
  
  GPIO_config(YLED);
  GPIO_config(GLED1); GPIO_config(RLED1);
  GPIO_config(GLED2); GPIO_config(RLED2);
  GPIO_config(BTN);
  
  UART_init(USART, USART_CLK_HSE, 16000000 * 4 / 115200);
  
  UART_puts(USART, __DATE__"  "__TIME__"\r\n");
  delay_ticks(500000);
  UART_puts(USART, "---\r\n");
  InterruptEnable();
  SystemInit();
  
  delay_ticks(500);
  lcd_init();
  lcd_rect(0,0, LCD_MAXX+1, LCD_MAXY+1, 0);

///////////////////////////////////////////////
  
  glInit();
  uint16_t i=0,j=0,k=0;
  uint8_t mode=0;
  uint8_t old_state=0;
  uint32_t t_av = 0, t_cur;
  uint32_t frames = 0;
  while(1){
    glLoadIdentity();
    
    glRotateZu(i>>8);
    glRotateXu(j>>8);
    glRotateYu(k>>8);
    
    i+=0xFF;
    j+=0x010F;
    k+=0x0250;
    
    if(mode == 0){
      glDrawTriangle(&v[0],&v[1],&v[2]);
      glDrawTriangle(&v[1],&v[2],&v[3]);
      glDrawTriangle(&v[4],&v[5],&v[6]);
      glDrawTriangle(&v[5],&v[6],&v[7]);
      glDrawTriangle(&v[8],&v[9],&v[10]);
      glDrawTriangle(&v[9],&v[10],&v[11]);
    }else if(mode == 1){
      Draw_lur();
    }else if(mode == 2){
      Draw_fly();
    }else if(mode == 3){
      Draw_shadow();
    }else{
      mode=0;
    }
    
    if(GPI_ON(BTN)){
      old_state=1;
    }else{
      if(old_state == 1){
        mode++;
        if(mode > 3)mode=0;
      }
      old_state = 0;
    }
      
    glSwapBuffers();
    GPO_T(RLED1);
    frames++;
    
    t_cur = systick_read32();
    if( (int32_t)(t_cur - t_av)  > 0 ){
      t_av = t_cur + 60000000;
      lcd_str(0, LCD_MAXY-20, 2, "Ê1921ÂÃ015  ");
      uint32_t fps = frames;
      lcd_str(-1,-1, 2, fpi32tos(NULL, frames, 0, 0));
      lcd_str(-1,-1, 2, " FPS  ");
      frames = 0;
    }
  }

}
