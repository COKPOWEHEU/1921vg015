#include <stdint.h>

void trap_entry();
int main(int argc, char **argv);
extern uint8_t __bss_start, __bss_end;
extern uint8_t __data_target_start, __data_target_end, __data_source_start;
extern uint8_t __sdata_target_start, __sdata_target_end, __sdata_source_start;

#define _TOSTR(s) #s
#define TOSTR(s) _TOSTR(s)

#define write_csr(reg, val) do{   \
    uint32_t temp = (uint32_t)val;\
    asm volatile(                 \
      "csrw " TOSTR(reg) ",%0"    \
      : : "rK" (temp)             \
      : "memory"                  \
    );                            \
  }while(0)

#define set_csr(reg, val) do{     \
    uint32_t temp = (uint32_t)val;\
    asm volatile(                 \
      "csrs " TOSTR(reg) ",%0"    \
      : : "rK" (temp)             \
      : "memory"                  \
    );                            \
  }while(0)

  
__attribute__((section(".startup.entry"))) __attribute__((noreturn)) void _start(){
  uint32_t temp;
  write_csr(mstatus, 0);
  asm volatile("la gp, __global_pointer$");
  write_csr(mtvec, trap_entry);
  write_csr(mie, 0);
  write_csr(mip, 0);
  set_csr(mstatus, (0b01<<13)); //FS=0b01, FPU status: initial
  asm volatile(
    "la sp, __STACK_END__ \n"
    "mv tp, sp            \n"
    "csrw mscratch, sp    \n"
  );
  
  for(uint8_t *bss=&__bss_start; bss<&__bss_end; bss++)bss[0] = 0;
  
  uint8_t *src = &__data_source_start;
  for(uint8_t *dst = &__data_target_start; dst<&__data_target_end; dst++){
    dst[0] = *(src++);
  }
  
  src = &__sdata_source_start;
  for(uint8_t *dst = &__sdata_target_start; dst<&__sdata_target_end; dst++){
    dst[0] = *(src++);
  }
  
  main(0, (void*)0);
  while(1){}
}


__attribute__((weak))void trap_entry(){
  while(1){}
}