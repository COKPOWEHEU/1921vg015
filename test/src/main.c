#include "K1921VG015.h"
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

char buf[1000];
char str[] = "some string\r\n";

int main(){
  RCU->CGCFGAHB |= RCU_CGCFGAHB_GPIOAEN_Msk;
  RCU->RSTDISAHB |= RCU_RSTDISAHB_GPIOAEN_Msk;
  GPIOA->OUTENSET = (1<<2 | 1<<12 | 1<<13 | 1<<14 | 1<<15);
  GPIOA->DATAOUT = 0;
  
  uart_init();
  
  uart_puts(__DATE__"  "__TIME__"\r\n");
  uart_puts(str);
  
  
  uint32_t test;
  //asm volatile("csrr  %0, misa" : "=r"(test));
  //uart_x32(test);
  
  //uart_x32(&__STACK_START__); uart_puts("\r\n");
  //uart_x32(&__C_STACK_TOP__); uart_puts("\r\n");
  //uart_x32(&__STACK_END__); uart_puts("\r\n");
  //uart_x32(&__TLS0_BASE__); uart_puts("\r\n");
  

  uint32_t t_prev = 0, t_cur;
  int idx = 0;
  while(1){
    if(uart_rxrdy()){
      char c = uart_getc();
      if(c == 'a')GPIOA->DATAOUTSET = (1<<15);
      if(c == 's')GPIOA->DATAOUTCLR = (1<<15);
    }
    
    t_cur = systick_read32();
    if( (t_cur - t_prev) > 100000){
      GPIOA->DATAOUTTGL = (1<<14);
      t_prev = t_cur;
    } 
    
  }
}
