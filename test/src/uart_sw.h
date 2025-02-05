#ifndef __UART_SW_H__
#define __UART_SW_H__
#warning F_CPU=1MHz ; baud=9600

#define UART_DT (F_CPU/RETARGET_UART_BAUD - 24)
void uart_putc(char c){
  GPIOA->DATAOUTCLR = (1<<1);
  delay_ticks(UART_DT);
  for(int i=0; i<8; i++){
    if(c & (1<<0))GPIOA->DATAOUTSET = (1<<1); else GPIOA->DATAOUTCLR = (1<<1);
    c>>=1;
    delay_ticks(UART_DT);
  }
  GPIOA->DATAOUTSET = (1<<1);
  delay_ticks(UART_DT);
}

void uart_init(){
  GPIOA->OUTENSET = (1<<1);
  GPIOA->DATAOUTSET = (1<<1);
  uart_putc(' ');
  delay_ticks(UART_DT * 100);
}


#endif