#include "K1921VG015.h"
#include "system_k1921vg015.h"
#include "plic.h"
#include "pinmacro.h"
#define USART 0
#include "clock.h" 
#include "uart.h"
#include "strlib.h"

#define YLED	A,2,1,GPIO_PP
#define GLED1	A,12,1,GPIO_PP
#define RLED1	A,13,1,GPIO_PP
#define GLED2	A,14,1,GPIO_PP
#define RLED2	A,15,1,GPIO_PP
#define BTN		A,11,1,GPIO_HIZ

void usb_reset(){
  //USB->USB_EP[0].START_ADDR= 0;
  //USB->USB_EP[0].END_ADDR= 0;
}

void USB_IRQ_Handler(){
  uint32_t status = USB->INTSTAT0;
  if(status & USB_INTSTAT0_USBBUSINT_Msk){
    status = USB->INTSTAT1;
    if(status & USB_INTSTAT1_RESSTATUS_Msk){
      GPO_T(RLED1);
      usb_reset();
      USB->INTSTAT1 = USB_INTEN1_RESSTATUS_Msk;
      return;
    }
  }
  GPO_T(GLED1);
  UART_puts(USART, "Unknown: ");
  UART_puts(USART, u32tohex(NULL, status)); UART_puts(USART, "\t");
  uint32_t res = USB->INTSTAT1;
  UART_puts(USART, u32tohex(NULL, res)); UART_puts(USART, "\r\n");
  PLIC_IntDisable(Plic_Mach_Target, IsrVect_IRQ_USB);
}

void usb_init(){
  USB->PLLUSBCFG3_bit.USBCLKSEL = 1;
  USB->INTEN0 = USB_INTEN0_USBBUSINTEN_Msk; //USB_INTEN0_CEP_INTEN_Msk;
  USB->INTEN1 = USB_INTEN1_RESSTATUS_Msk;
  
  PLIC_SetIrqHandler(Plic_Mach_Target, IsrVect_IRQ_USB, USB_IRQ_Handler);
  PLIC_SetPriority(IsrVect_IRQ_USB, 1);
  PLIC_IntEnable(Plic_Mach_Target, IsrVect_IRQ_USB);
}

int main(){
  RCU->CGCFGAHB |= RCU_CGCFGAHB_GPIOAEN_Msk | RCU_CGCFGAHB_GPIOBEN_Msk | RCU_CGCFGAHB_GPIOCEN_Msk;
  RCU->RSTDISAHB |= RCU_RSTDISAHB_GPIOAEN_Msk | RCU_RSTDISAHB_GPIOBEN_Msk | RCU_RSTDISAHB_GPIOCEN_Msk;
  GPIOA->DATAOUTCLR = 0xFF; GPIOB->DATAOUTCLR = 0xFF; GPIOC->DATAOUTCLR = 0xFF;
  GPIOA->ALTFUNCCLR = 0xFF; GPIOB->ALTFUNCCLR = 0xFF; GPIOC->ALTFUNCCLR = 0xFF; 
  GPIOA->OUTENCLR = 0xFF; GPIOB->OUTENCLR = 0xFF; GPIOC->OUTENCLR = 0xFF;
  GPIOA->ALTFUNCNUM = 0; GPIOB->ALTFUNCNUM = 0; GPIOC->ALTFUNCNUM = 0;
  
  
  GPIO_config(YLED);
  GPIO_config(GLED1); GPIO_config(RLED1);
  GPIO_config(GLED2); GPIO_config(RLED2);
  GPIO_config(BTN);
  
  //UART_init(USART, USART_CLK_HSE, 16000000 * 4 / 115200);
  //UART_puts(USART, __DATE__"  "__TIME__"\r\n");
  delay_ticks(500000);
  UART_puts(USART, "---\r\n");
  InterruptEnable();
  
  while(GPI_ON(BTN)){}
  SystemInit();
  
  UART_init(USART, USART_CLK_SYSPLL0, 60000000 * 4 / 115200);
  UART_puts(USART, __DATE__"  "__TIME__"\r\n");
  
/////////////////////////////////////////////////////
  RCU->CGCFGAHB |= RCU_CGCFGAHB_USBEN_Msk;
  RCU->RSTDISAHB |= RCU_RSTDISAHB_USBEN_Msk;
  usb_init();
  
/////////////////////////  

  uint32_t t_cur, t_av = 0;
  while(1){
    t_cur = systick_read32();
    if( (int32_t)(t_cur - t_av) > 0){
      GPO_T(YLED);
      t_av = t_cur + 5000000;
    }
  }
}
