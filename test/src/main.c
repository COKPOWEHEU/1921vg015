#include "K1921VG015.h"
#include "system_k1921vg015.h"
#include "plic.h"
#define F_CPU	1000000
#define RETARGET_UART_BAUD 115200
#include "clock.h" 
#include "uart_hw.h"

void uart_puts(char *s){
  while(s[0]!=0){uart_putc(s[0]); s++;}
}

void uart_x8(uint8_t val){
  uint8_t h = (val >> 4);
  val &= 0xF;
  if(h <= 9)uart_putc(h+'0'); else uart_putc(h-0x0A+'A');
  if(val<=9)uart_putc(val+'0');else uart_putc(val-0x0A+'A');
}
void uart_x32(uint32_t val){
  uart_x8(val>>24);
  uart_x8(val>>16);
  uart_x8(val>>8);
  uart_x8(val);
}
void uart_i32(int32_t val){
  char buf[12];
  char *ch = &(buf[11]);
  ch[0] = 0;
  if(val < 0){buf[0]='-'; val=-val;}else buf[0] = 0;
  do{
    ch--;
    ch[0] = (val % 10) + '0';
    val /= 10;
  }while(val);
  if(buf[0]=='-'){ch--; ch[0]='-';}
  uart_puts(ch);
}


void InterruptEnable(){
  //allow all interrupts in machine mode
  PLIC_SetThreshold (Plic_Mach_Target, 0); //allow all interrupts in machine mode
  // disable timer interrupt
  clear_csr(mie, MIE_MTIMER);
  // enable machine external interrupt
  set_csr(mie, MIE_MEXTERNAL);
  // enable global interrupts
  set_csr(mstatus, MSTATUS_MIE);
}

void InterruptDisable(){
  //disable all interrupts in machine mode
  PLIC_SetThreshold (Plic_Mach_Target, 7); //Disable all interrupts in machine mode
  // disable machine external interrupt
  clear_csr(mie, MIE_MEXTERNAL);
  // disable global interrupts
  clear_csr(mstatus, MSTATUS_MIE);
}

/*
__attribute__((interrupt))void trap_entry(){
  uint8_t t = PLIC->MICC;
  if(t == IsrVect_IRQ_UART0){
    GPIOA->DATAOUTTGL = (1<<12);
    char ch = UART0->DR_bit.DATA;
    if(ch == 'a')GPIOA->DATAOUTSET = (1<<15);
    if(ch == 's')GPIOA->DATAOUTCLR = (1<<15);
    //UART0->ICR = UART_ICR_RXIC_Msk;
  }
  PLIC->MICC = IsrVect_IRQ_UART0;
}*/
void UART0_IRQHandler(){
  GPIOA->DATAOUTTGL = (1<<12);
  char ch = UART0->DR_bit.DATA;
  if(ch == 'a')GPIOA->DATAOUTSET = (1<<15);
  if(ch == 's')GPIOA->DATAOUTCLR = (1<<15);
}

int main(){
  RCU->CGCFGAHB |= RCU_CGCFGAHB_GPIOAEN_Msk;
  RCU->RSTDISAHB |= RCU_RSTDISAHB_GPIOAEN_Msk;
  GPIOA->OUTENSET = (1<<2 | 1<<12 | 1<<13 | 1<<14 | 1<<15);
  GPIOA->DATAOUT = 0;
  
  uart_init();
  
  uart_puts(__DATE__"  "__TIME__"\r\n");
  
  UART0->IMSC = UART_IMSC_RXIM_Msk;
  PLIC_SetIrqHandler (Plic_Mach_Target, IsrVect_IRQ_UART0, UART0_IRQHandler);
  PLIC_SetPriority   (IsrVect_IRQ_UART0, 1);
  PLIC_IntEnable     (Plic_Mach_Target, IsrVect_IRQ_UART0);
    
  InterruptEnable();

  uint32_t t_prev = 0, t_cur, t_av = 0;
  int idx = 0;
  while(1){
    /*if(uart_rxrdy()){
      char c = uart_getc();
      if(c == 'a')GPIOA->DATAOUTSET = (1<<15);
      if(c == 's')GPIOA->DATAOUTCLR = (1<<15);
    }*/
    
    t_cur = systick_read32();
    if( (int32_t)(t_cur - t_av) > 0){
      GPIOA->DATAOUTTGL = (1<<14);
      t_av = t_cur + 100000;
    }
    
  }
}
