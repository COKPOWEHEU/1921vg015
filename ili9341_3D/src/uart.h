#ifndef __UART_HW__
#define __UART_HW__

#if 1==0

#define USART 0 //USART0
//#define USART_REMAP
#define UART_SIZE_PWR 8 // 256-bytes ring buffer
#include "uart.h"
  ...
//Init selected UART. Internal speed if 4 times larget than reference clock
UART_init(USART, USART_CLK_HSE, 16000000 * 4 / 115200); 
UART_puts(USART, __TIME__ " " __DATE__ "\r\n");
int32_t sz = UART_str_size(USART);
if(sz > 0){
  char buf[100];
  UART_read(USART, buf, sz);
}else if(sz < 0){ //error: fifo is fill but '\r' or '\n' not found
  UART_rx_clear(USART);
}

#endif

typedef enum{
  USART_CLK_HSI = 0b11,
  USART_CLK_HSE = 0b01,
  USART_CLK_SYSPLL0=0b10,
  USART_CLK_SYSPLL1=0b11,
}USART_CLK_t;

#ifndef UART_PIN
  #define _UART_PIN(num,dir) UART ## num ## _ ## dir
  #define UART_PIN(num,dir) _UART_PIN(num,dir)
#endif

#define _UARTn_func(n, func) UART ## n ## _ ## func
#define UARTn_func(n, func) _UARTn_func(n, func)


#define UART_init(num, clk, brr) _UART_init(num, clk, brr)
#define UART_received(num)	_UART_received(num)
#define UART_str_size(num) _UART_str_size(num)
#define UART_avaible(num)  _UART_avaible(num)
#define UART_getc(num)  	_UART_getc(num)
#define UART_read(num, data, len) _UART_read(num, data, len)
#define UART_gets(num, str, len) _UART_gets(num, str, len)
#define UART_rx_clear(num) _UART_rx_clear(num)
#define UART_write(num, data, len) _UART_write(num, data, len)
#define UART_puts(num, str) _UART_puts(num, str)

#define _UART_init(num, clk, brr) UART ## num ## _init(clk, brr)
#define _UART_received(num)	(uart_buf_busy(&_uartN_Dx(num,rx)))
#define _UART_avaible(num)  (uart_buf_free(&_uartN_Dx(num,tx)))
#define _UART_str_size(num) (uart_buf_str_size( &_uartN_Dx(num,rx) ) )
#define _UART_getc(num)  	uart_buf_getc(&_uartN_Dx(num,rx))
#define _UART_read(num, data, len) UART##num##_read(data, len)
#define _UART_gets(num, str, len) uart_buf_gets(&_uartN_Dx(num,rx), str, len)
#define _UART_rx_clear(num) do{_uartN_Dx(num,rx).en = _uartN_Dx(num,rx).st;}while(0)
#define _UART_write(num, data, len) UART##num##_write(data, len)
#define _UART_puts(num, str) UART##num##_puts(str)



void UARTn_func(USART, init)(USART_CLK_t clk, uint32_t brr);


#ifdef USART

#if (USART == 0) && !defined(UART0_TX)
  #ifndef USART_REMAP
    #define UART0_TX		A,1,1,GPIO_AF1
    #define UART0_RX		A,0,1,GPIO_AF1
  #else
    #define UART0_TX		B,7,1,GPIO_AF3  
    #define UART0_RX		B,6,1,GPIO_AF3
  #endif
#endif

#if (USART == 1) && !defined(UART1_TX)
  #ifndef USART_REMAP
    #define UART1_TX		A,3,1,GPIO_AF1
    #define UART1_RX		A,2,1,GPIO_AF1
  #else
    #define UART1_TX		A,11,1,GPIO_AF3  
    #define UART1_RX		A,10,1,GPIO_AF3  
  #endif
#endif

#if (USART == 2) && !defined(UART2_TX)
  #ifndef USART_REMAP
    #define UART2_TX		A,5,1,GPIO_AF1
    #define UART2_RX		A,4,1,GPIO_AF1
  #else
    #define UART2_TX		A,13,1,GPIO_AF3
    #define UART2_RX		A,12,1,GPIO_AF3
  #endif
#endif

#if (USART == 3) && !defined(UART3_TX)
  #ifndef USART_REMAP
    #define UART3_TX		A,7,1,GPIO_AF1
    #define UART3_RX		A,6,1,GPIO_AF1
  #else
    #define UART3_TX		A,15,1,GPIO_AF3
    #define UART3_RX		A,14,1,GPIO_AF3
  #endif
#endif

#if (USART == 4) && !defined(UART4_TX)
  #ifndef USART_REMAP
    #define UART4_TX		A,9,1,GPIO_AF1
    #define UART4_RX		A,8,1,GPIO_AF1
  #else
    #define UART4_TX		B,1,1,GPIO_AF3
    #define UART4_RX		B,0,1,GPIO_AF3
  #endif
#endif


#include "pinmacro.h"

#ifndef UART_SIZE_PWR
  #define UART_SIZE_PWR 6
#endif
#ifndef NULL
  #define NULL ((void*)0)
#endif
#define _uartN_Dx(num, dir) uart ## num ## _ ## dir
#define uartN_Dx(num, dir) _uartN_Dx(num, dir)

#define UART_SIZE (1<<UART_SIZE_PWR)
#define UART_MASK (UART_SIZE-1)

typedef struct{
  volatile uint16_t st,en;
  volatile uint8_t arr[UART_SIZE];
}uart_buffer;

uart_buffer uartN_Dx(USART, rx); //uart1_rx
uart_buffer uartN_Dx(USART, tx); //uart1_tx

static uint16_t uart_buf_busy(uart_buffer *buf){
  return ((buf->st - buf->en) & UART_MASK);
}

#define uart_buf_avaible(buf) (UART_SIZE - uart_buf_busy(buf) - 1)

static uint8_t uart_buf_getc(uart_buffer *buf){
  uint8_t res;
  if(uart_buf_busy(buf) == 0)return 0;
  res = buf->arr[buf->en];
  buf->en++;
  buf->en &= UART_MASK;
  return res;
}

static void uart_buf_putc(uart_buffer *buf, uint8_t dat){
  if(uart_buf_avaible(buf) < 1)return;
  buf->arr[buf->st]=dat;
  buf->st++;
  buf->st &= UART_MASK;
}

static int32_t uart_buf_str_size(uart_buffer *buf){
  uint32_t en = buf->en;
  uint8_t *arr = (uint8_t*)(buf->arr);
  uint32_t sz = (buf->st - en) & UART_MASK;
  uint32_t pos;
  if(sz == UART_SIZE - 1)return -1;
  for(uint32_t i=0; i<sz; i++){
    pos = (i + en) & UART_MASK;
    if((arr[pos] == '\r') || (arr[pos] == '\n'))return i+1;
  }
  return 0;
}

static char *uart_buf_gets(uart_buffer *buf, char *str, uint32_t len){
  uint32_t en = buf->en;
  uint8_t *arr = (uint8_t*)(buf->arr);
  uint32_t sz = (buf->st - en) & UART_MASK;
  uint32_t strsz;
  uint32_t pos;
  str[0] = 0;
  if(sz > len)sz = len;
  if(sz < 2)return NULL;
  for(strsz = 1; strsz < sz; strsz++){ // find '\r\n' in buffer
    pos = (strsz + en) & UART_MASK;
    if((arr[pos] == '\r') || (arr[pos] == '\n'))break;
  }
  if((arr[pos] != '\r') && (arr[pos] != '\n')){ // '\r\n' not found
    if(strsz < len)return NULL; // if user requests less bytes then return received part
  }
  
  if((arr[en] == '\r') || (arr[en] == '\n')){ //remove '\r\n' from start
    en = (en+1)&UART_MASK;
    strsz--;
  }
  
  for(uint32_t i=0; i<strsz; i++){
    str[i] = arr[ (i+en)&UART_MASK ];
  }
  str[strsz] = 0;
  
  buf->en = (strsz+en) & UART_MASK;
  return str;
}


#define _UART(num) UART ## num
#define UART(num) _UART(num)



#define UART_wait(num) _UART_wait(num)
#define _UART_wait(num) UART ## num ## _wait()
//UART_wait
void UARTn_func(USART, wait)(){
  UART(USART)->CR = 0;
  while( UART(USART)->FR_bit.BUSY ){}
  UART(USART)->LCRH = 0;
}
#define _uart_IRQ(num) USART ## num ## _IRQHandler
#define uart_IRQ(num) _uart_IRQ(num)
void uart_IRQ(USART)(void);

//UART_init
void UARTn_func(USART, init)(USART_CLK_t clk, uint32_t brr){
  RCU->CGCFGAPB |= concat3(RCU_CGCFGAPB_UART, USART, EN_Msk);
  RCU->RSTDISAPB |= concat3(RCU_RSTDISAPB_UART, USART, EN_Msk);
  
  UART_wait(USART);
  
  GPIO_config(UART_PIN(USART,RX));
  GPIO_manual(UART_PIN(USART,TX), GPIO_HIZ);
  
  RCU->UARTCLKCFG[USART].UARTCLKCFG = 
    (clk << RCU_UARTCLKCFG_CLKSEL_Pos) |
    RCU_UARTCLKCFG_CLKEN_Msk |
    RCU_UARTCLKCFG_RSTDIS_Msk;

  UART(USART)->IBRD = brr / 64;
  UART(USART)->FBRD = brr & 63;
  
  UART(USART)->LCRH = (0b11 << UART_LCRH_WLEN_Pos); //8-bit
  
  UART(USART)->CR = UART_CR_TXE_Msk | UART_CR_RXE_Msk | UART_CR_UARTEN_Msk;
  
  UART(USART)->IMSC = UART_IMSC_RXIM_Msk;
  PLIC_SetIrqHandler(Plic_Mach_Target, concat2(IsrVect_IRQ_UART, USART), uart_IRQ(USART));
  PLIC_SetPriority(concat2(IsrVect_IRQ_UART, USART), 1);
  PLIC_IntEnable(Plic_Mach_Target, concat2(IsrVect_IRQ_UART, USART));
  
  //Костыль: UART не взводит флаг TXRIS пока не будет передан хоть один байт
  UART(USART)->DR = '!';
  while( UART0->FR_bit.BUSY ){}
  delay_ticks(1);
  
  GPIO_config(UART_PIN(USART,TX));
}

#define UART_speed(num, clk, brr) do{\
    RCU->UARTCLKCFG[USART].UARTCLKCFG_bit.CLKSEL = clk; \
    UART(USART)->IBRD = brr / 64; \
    UART(USART)->FBRD = brr & 63; \
  }while(0)
  
#define UART_parity_disable(num) do{UART(USART)->LCRH_bit.PEN = 0;}while(0)
#define UART_parity_even(num) do{ \
    UART(USART)->LCRH_bit.PEN = 1; \
    UART(USART)->LCRH_bit.EPS = 1; \
  }while(0)
#define UART_parity_odd(num) do{ \
    UART(USART)->LCRH_bit.PEN = 1; \
    UART(USART)->LCRH_bit.EPS = 0; \
  }while(0)

// Interrupt
#define _uart_IRQ(num) USART ## num ## _IRQHandler
#define uart_IRQ(num) _uart_IRQ(num)
void uart_IRQ(USART)(void){
  if( UART(USART)->RIS_bit.RXRIS ){
    char ch = UART(USART)->DR_bit.DATA;
    uart_buf_putc(&uartN_Dx(USART, rx), ch);
  }else if( UART(USART)->RIS_bit.TXRIS ){
    if(uart_buf_busy(&uartN_Dx(USART, tx)) != 0){
      UART(USART)->DR = uart_buf_getc(&uartN_Dx(USART, tx));
    }else{
      UART(USART)->IMSC_bit.TXIM = 0;
    }
  }
}

#define UART_putc(num, data) do{\
  uart_buf_putc(&_uartN_Dx(num,tx), data);\
  UART(USART)->IMSC_bit.TXIM = 1;\
}while(0)

//UART_write
void UARTn_func(USART, write)(uint8_t *data, uint32_t len){
  while(len--){
    uart_buf_putc(&uartN_Dx(USART,tx), data[0]);
    data++;
  }
  UART(USART)->IMSC_bit.TXIM = 1;
}

//UART_puts
void UARTn_func(USART, puts)(char *str){
  while(str[0] != 0){
    uart_buf_putc(&uartN_Dx(USART,tx), str[0]);
    str++;
  }
  UART(USART)->IMSC_bit.TXIM = 1;
}

//UART_read
uint8_t* UARTn_func(USART, read)(uint8_t *data, uint32_t len){
  data[0] = 0;
  if(UART_received(USART) < len)return NULL;
  while(len--){
    *(data++) = UART_getc(USART);
  }
  return data;
}

#endif // USART defined

#endif