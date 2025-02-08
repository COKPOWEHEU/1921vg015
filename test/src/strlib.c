#include "strlib.h"

#define NUMLEN(val) (\
  (val)<100000?( /* <10⁵ */ \
    (val)< 100?( \
      (val)<10? 1 : 2 \
    ):( /*  10³ / 10⁴ */ \
      (val)<10000? (val)<1000? 3 : 4 : 5 \
    ) \
  ):( /* 10⁶ ... 10¹⁰ */\
    (val) < 10000000?( /* <10⁸ */ \
      (val)<1000000? 6 : 7 \
    ):( /* 10⁸, 10⁹, 10¹⁰ */ \
      (val)<1000000000? (val)<100000000? 8 : 9 : 10 \
    ) \
  ) \
  )

static char strlib_buf[13];

char* utobin(char *buf, uint32_t val, uint8_t bits){
  if(buf == NULL)buf = strlib_buf;
  char *ch = buf;
  for(int i=(1<<(bits-1)); i!=0; i>>=1){
    if(val & i)ch[0]='1'; else ch[0]='0';
    ch++;
  }
  ch[0] = 0;
  return buf;
}

char* fpi32tos(char *buf, int32_t val, uint8_t dot, int8_t field){
  if(buf == NULL)buf = strlib_buf;
  buf[0] = 0;
  char *ch = &buf[13];
  ch--; ch[0] = 0;
  if(val < 0){buf[0]='-'; val=-val;}
  for(int i=0; i<dot; i++){
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }
  if(dot > 0){ch--; ch[0]='.';}
  do{
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }while(val);
  if(buf[0] != 0){ch--; ch[0] = '-';}
  field -= 12-(ch-buf);
  for(;field>0;field--){ch--; ch[0] = ' ';}
  return ch;
}

char* fpi32tos_inplace(char *buf, int32_t val, uint8_t dot, int8_t field){
  static char strlib_buf[13];
  if(buf == NULL)buf = strlib_buf;
  uint32_t len = val;
  if(val < 0)len = -val;
  len = NUMLEN(len);
  if(dot >= len)len = dot + 2; else if(dot > 0)len++;
  if(val < 0)len++;
  
  if( len > field ){
    if( field == 0 ){
      field = len;
    }else{
      if(val > 0){
        for(uint32_t i=0; i<field; i++)buf[i] = '+';
      }else{
        for(uint32_t i=0; i<field; i++)buf[i] = '-';
      }
      return buf;
    }
  }
  uint32_t empty = field - len;
  for(int i=0; i<empty; i++)buf[i] = ' ';
  if(val < 0){val = -val; buf[empty] = '-';}
  
  char *ch = &buf[field];
  
  for(int i=0; i<dot; i++){
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }
  if(dot > 0){ch--; ch[0]='.';}
  do{
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }while(val);

  return &buf[field];
}

char* u32tohex(char *buf, uint32_t val){
  if(buf == NULL)buf = strlib_buf;
  for(int i=0; i<8; i++){
    uint32_t b = val >> 28;
    if(b < 0xA)buf[i] = b+'0'; else buf[i] = b-0xA+'A';
    val <<= 4;
  }
  buf[8] = 0;
  return buf;
}