#ifndef __DMA_H__
#define __DMA_H__

extern __attribute__((aligned(1024))) DMA_CtrlData_TypeDef DMA_CONFIGDATA;

#define DMA_init_macro() \
  void dma_init(){ \
    DMA->BASEPTR = (uint32_t)(&DMA_CONFIGDATA); \
    DMA->CFG_bit.MASTEREN = 1; \
    DMA->ENCLR = 0xFFFFFFFF; \
  } \
  __attribute__((aligned(1024))) DMA_CtrlData_TypeDef DMA_CONFIGDATA
  
#define _DMA_CH(dma, ...) DMA_CONFIGDATA.PRM_DATA.CH[dma]
#define DMA_CH(dma) _DMA_CH(dma)
  
#define dma_clock(...) //compatibility

#define dma_cfg_io(dma, dst, src, count) do{ \
    _DMA_CH(dma).SRC_DATA_END_PTR = (uint32_t)src; \
    _DMA_CH(dma).DST_DATA_END_PTR = (uint32_t)dst; \
    _DMA_CH(dma).CHANNEL_CFG_bit.N_MINUS_1 = count-1; \
    if(0)(( void(*)(volatile void*, volatile void*) )\
      "dma_cfg_io(void*, void*)")(dst, src); \
  }while(0)
  
#define datasztoflags(sz) (sz==8? 0 :sz==16? 1 :sz==32? 2 : 3)

#define dma_cfg_mem(dma, dstsize, dstinc, srcsize, srcinc, circ, prior) do{ \
    if(srcinc){ \
      _DMA_CH(dma).SRC_DATA_END_PTR += _DMA_CH(dma).CHANNEL_CFG_bit.N_MINUS_1; \
      _DMA_CH(dma).CHANNEL_CFG_bit.SRC_INC = datasztoflags(srcsize); \
    }else{ \
      _DMA_CH(dma).CHANNEL_CFG_bit.SRC_INC = DMA_CHANNEL_CFG_SRC_INC_None; \
    }\
    _DMA_CH(dma).CHANNEL_CFG_bit.SRC_SIZE = datasztoflags(srcsize); \
    if(dstinc){ \
      _DMA_CH(dma).DST_DATA_END_PTR += _DMA_CH(dma).CHANNEL_CFG_bit.N_MINUS_1; \
      _DMA_CH(dma).CHANNEL_CFG_bit.DST_INC = datasztoflags(dstsize); \
    }else{ \
      _DMA_CH(dma).CHANNEL_CFG_bit.DST_INC = DMA_CHANNEL_CFG_SRC_INC_None; \
    }\
    _DMA_CH(dma).CHANNEL_CFG_bit.DST_SIZE = datasztoflags(dstsize); \
    _DMA_CH(dma).CHANNEL_CFG_bit.R_POWER = 0; \
  }while(0)
  
      
#define dma_enable(dma) do{ \
    _DMA_CH(dma).CHANNEL_CFG_bit.CYCLE_CTRL = DMA_CHANNEL_CFG_CYCLE_CTRL_Basic; \
    DMA->ENSET = (1<<_marg1(dma)); \
    _marg2(dma); \
  }while(0)
  
#define dma_disable(dma) do{ \
    _marg3(dma);\
    DMA->ENCLR = (1<<_marg1(dma)); \
  }while(0)



#endif