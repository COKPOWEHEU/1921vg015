#ifndef __CLOCK_H__
#define __CLOCK_H__

uint32_t systick_read32(){
  uint32_t res;
  asm volatile("csrr %0, time" : "=r"(res) );
  return res;
}

uint64_t systick_read64(){
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

void delay_ticks(uint32_t dt){
  uint32_t t_av = systick_read32() + dt;
  while( (int32_t)(t_av - systick_read32()) > 0 ){}
}

/*void delay_ticks1(uint32_t dt){
  uint32_t t_prev = systick_read32();
  while( (systick_read32() - t_prev) < dt){}
}*/

#endif