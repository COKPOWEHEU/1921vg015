#ifndef __CLOCK_H__
#define __CLOCK_H__

static inline uint32_t systick_read32(){
  uint32_t res;
  asm volatile("csrr %0, time" : "=r"(res) );
  return res;
}

static inline uint64_t systick_read64(){
  uint32_t resl, resh1, resh2;
  asm volatile(
    "csrr %0, timeh		\r\n"
    "csrr %1, time		\r\n"
    "csrr %2, timeh		\r\n"
    "beq %0, %2, 1f		\r\n"
    "  csrr %1, time	\r\n"
    "1:					\r\n"
    : "=r"(resh1), "=r"(resl), "=r"(resh2) );
  return (((uint64_t)resh2)<<32) | resl;
}

static inline void delay_ticks(uint32_t dt){
  uint32_t t_av = systick_read32() + dt;
  while( (int32_t)(t_av - systick_read32()) > 0 ){}
}

/*void delay_ticks1(uint32_t dt){
  uint32_t t_prev = systick_read32();
  while( (systick_read32() - t_prev) < dt){}
}*/


#define __TIME_S__ ((__TIME__[0]-'0')*36000+(__TIME__[1]-'0')*3600 + (__TIME__[3]-'0')*600+(__TIME__[4]-'0')*60 + (__TIME__[6]-'0')*10+(__TIME__[7]-'0'))
#define __DATE_D__ (((__DATE__[4]==' '?'0':__DATE__[4])-'0')*10 + (__DATE__[5]-'0'))
#define __DATE_M__ (((__DATE__[1]=='a')&&(__DATE__[2]=='n'))?1: \
                    (__DATE__[2]=='b')?2: \
                    ((__DATE__[1]=='a')&&(__DATE__[2]=='r'))?3: \
                    ((__DATE__[1]=='p')&&(__DATE__[2]=='r'))?4: \
                    (__DATE__[2]=='y')?5: \
                    ((__DATE__[1]=='u')&&(__DATE__[2]=='n'))?6: \
                    (__DATE__[2]=='l')?7: \
                    (__DATE__[2]=='g')?8: \
                    (__DATE__[2]=='p')?9: \
                    (__DATE__[2]=='t')?10: \
                    (__DATE__[2]=='v')?11: \
                    (__DATE__[2]=='c')?12: \
                   0)
#define __DATE_Y__ ((__DATE__[7]-'0')*1000 + (__DATE__[8]-'0')*100 + (__DATE__[9]-'0')*10 + (__DATE__[10]-'0'))
#ifndef DATETIME_TIMEZONE_S
  #define DATETIME_TIMEZONE_S	(3*3600)
#endif
#define _DATETIME_UTCOFFSET 951868800
//При расчете продолжительности года удобнее перенести февраль в конец года.
//Тогда длительность до текущего месяца вычисляется из массива
//const uint16_t day_offset[12] = {0, 31, 61, 92, 122, 153, 184, 214, 245, 275,306, 337};
//но в макрос нельзя встроить массив, поэтому длительность будем вычислять через кучу if-ов (снова от января)
#define _DAY_OFFSET(m) ((m)==0?275:(m)==1?306:(m)==2?337:(m)==3?0:(m)==4?31:(m)==5?61:(m)==6?92:(m)==7?122:(m)==8?153:(m)==9?184:(m)==10?214:245)
//длительность текущего года (в днях) с учетом високосности
#define __DATETIME_YEAR_DAYS(y) ((y)*365 + (y)/4 - (y)/100 + (y)/400)
//корректировка на начало года с марта
#define _DATETIME_YEAR_DAYS(m, y) __DATETIME_YEAR_DAYS((y)-2000-((m)<3))
#define UTC(h,m,s,dd,mm,yyyy) (((dd) - 1 + _DAY_OFFSET(mm) + _DATETIME_YEAR_DAYS(mm, yyyy)) * 86400 + (h)*3600 + (m)*60 + (s) + _DATETIME_UTCOFFSET - DATETIME_TIMEZONE_S)
#define __UTC__ UTC(0,0,__TIME_S__, __DATE_D__, __DATE_M__, __DATE_Y__)

#endif