#ifndef __UART_HW__
#define __UART_HW__

void uart_init(){
  RCU->CGCFGAPB_bit.UART0EN = 1;
  RCU->RSTDISAPB_bit.UART0EN = 1;
  GPIOA->ALTFUNCNUM_bit.PIN0 = 1;
  GPIOA->ALTFUNCNUM_bit.PIN1 = 1;
  GPIOA->ALTFUNCSET = (1 << 0) | (1 << 1);
  
  UART0->CR = 0;
  while( UART0->FR_bit.BUSY ){}
  UART0->LCRH = 0;
  
#ifdef F_HSE
  RCU->UARTCLKCFG[0].UARTCLKCFG = (1 << RCU_UARTCLKCFG_CLKSEL_Pos) |                                               	  	  RCU_UARTCLKCFG_CLKEN_Msk | RCU_UARTCLKCFG_RSTDIS_Msk;
  uint32_t brr = (F_HSE * 64 / RETARGET_UART_BAUD + 8)/16;
#else
  RCU->UARTCLKCFG[0].UARTCLKCFG = (0b00 << RCU_UARTCLKCFG_CLKSEL_Pos) | RCU_UARTCLKCFG_CLKEN_Msk | RCU_UARTCLKCFG_RSTDIS_Msk; 
  #warning: F_CPU не 1 МГц!
  #undef F_CPU
  #define F_CPU 946970
  uint32_t brr = (F_CPU * 64 / RETARGET_UART_BAUD)/16;
#endif
  
  UART0->IBRD = brr / 64;
  UART0->FBRD = brr & 63;
  
  UART0->LCRH = (0b11 << UART_LCRH_WLEN_Pos); //8-bit
  
  UART0->CR = UART_CR_TXE_Msk | UART_CR_RXE_Msk | UART_CR_UARTEN_Msk;
}

void uart_putc(char c){
  while( UART0->FR_bit.BUSY ){}
  UART0->DR = c;
}

char uart_rxrdy(){
  return !UART0->FR_bit.RXFE;
}

int uart_getc(){
  if(!uart_rxrdy())return -1;
  return UART0->DR_bit.DATA;
}

#endif