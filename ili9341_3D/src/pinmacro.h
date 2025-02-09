#ifndef __PINMACRO_H__
#define __PINMACRO_H__

#if 1==0

#define LED	A,12,1,GPIO_PP
#define BTN	A,11,1,GPIO_HIZ | GPIO_PULLUP
...
GPIO_config(LED);
GPIO_manual(BTN, GPIO_HIZ);
if( GPI_ON(BTN) )GPO_ON(LED); else GPO_OFF(LED);

#endif

#define GPIO_PP		0b00
#define GPIO_OD		0b01
#define GPIO_OS		0b10
#define GPIO_HIZ	0b11
#define GPIO_PULLUP	0b100

#define GPIO_AF1	0b01000
#define GPIO_AF2	0b10000
#define GPIO_AF3	0b11000
#define GPIO_AFMASK	0b11000

#define PM_BITMASK( reg, mask, val ) do{ (reg) = ((reg) &~ (mask)) | (((mask) &~((mask)<<1))*(val)); }while(0)

#define _concat2(a,b,...)  a ## b
#define _concat3(a,b,c,...)  a ## b ## c
#define concat2(a,b,...) _concat2(a,b)
#define concat3(a,b,c,...) _concat3(a,b,c)

#define _marg1(a,...)  a
#define marg1(x) _marg1(x)
#define _marg2(a,b,...)  b
#define marg2(x) _marg2(x)
#define _marg3(a,b,c,...)  c
#define marg3(x) _marg3(x)
#define _marg4(a,b,c,d,...)  d
#define marg4(x) _marg4(x)
#define _marg5(a,b,c,d,e,...)  e
#define marg5(x) _marg5(x)
#define GPIO(x) concat2(GPIO,x)

#define _GPIO_config(port, bit, act, mode) do{\
    if( ((mode) & GPIO_AFMASK) ){ \
      GPIO(port)->ALTFUNCSET = (1<<bit); \
      PM_BITMASK(GPIO(port)->ALTFUNCNUM, (0b11<<(bit*2)), ((mode>>3)&0b11) ); \
    }else{\
      if(((mode) & 0b11) == GPIO_HIZ){\
        GPIO(port)->OUTENCLR = (1<<bit);\
      }else{\
        GPIO(port)->OUTENSET = (1<<bit);\
        PM_BITMASK(GPIO(port)->OUTMODE, (0b11<<(bit*2)), ((mode)&0b11)); \
      } \
      if( (mode) & GPIO_PULLUP ){ \
        GPIO(port)->PULLMODE |= (1<<bit);\
      }\
    }\
  }while(0)
  
#define GPIO_config(descr) _GPIO_config(descr)
#define GPIO_manual(descr, mode) _GPIO_config(_marg1(descr), _marg2(descr), _marg3(descr), mode)
  
#define GPIO_AF_enable(descr) do{\
    GPIO(_marg1(descr))->ALTFUNCSET = (1<<_marg2(descr)); \
    PM_BITMASK(GPIO(_marg1(descr))->ALTFUNCNUM, (0b11<<(_marg2(descr)*2)), (_marg4(descr)>>3) ); \
  }while(0)
#define GPIO_AF_disable(descr) do{\
    GPIO(_marg1(descr))->ALTFUNCCLR = (1<<_marg2(descr)); \
  }while(0)
  
#define GPO_0(descr) do{\
    GPIO(_marg1(descr))->DATAOUTCLR = (1<<_marg2(descr)); \
  }while(0)
  
#define GPO_1(descr) do{\
    GPIO(_marg1(descr))->DATAOUTSET = (1<<_marg2(descr)); \
  }while(0)

#define GPO_ON(descr) do{\
    if(_marg3(descr)){ \
      GPIO(_marg1(descr))->DATAOUTSET=(1<<_marg2(descr));\
    }else{ \
      GPIO(_marg1(descr))->DATAOUTCLR=(1<<_marg2(descr)); \
    }\
  }while(0)
  
#define GPO_OFF(descr) do{\
    if(_marg3(descr)){ \
      GPIO(_marg1(descr))->DATAOUTCLR=(1<<_marg2(descr));\
    }else{ \
      GPIO(_marg1(descr))->DATAOUTSET=(1<<_marg2(descr)); \
    }\
  }while(0)
  
#define GPO_T(descr) do{\
    GPIO(_marg1(descr))->DATAOUTTGL=(1<<_marg2(descr)); \
  }while(0)
  
#define GPI_1(descr) ( GPIO(_marg1(descr))->DATA & (1<<_marg2(descr)) )
#define GPI_0(descr) (!( GPIO(_marg1(descr))->DATA & (1<<_marg2(descr)) ))
  
#define GPI_ON(descr)  ( (GPIO(_marg1(descr))->DATA & (1<<_marg2(descr))) == (_marg3(descr)<<_marg2(descr)) )
#define GPI_OFF(descr) ( (GPIO(_marg1(descr))->DATA & (1<<_marg2(descr))) != (_marg3(descr)<<_marg2(descr)) )

#endif