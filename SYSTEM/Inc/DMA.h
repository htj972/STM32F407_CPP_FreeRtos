/**
* @Author kokirika
* @Name DMA
* @Date 2022-09-16
**/

#ifndef _DMA_H
#define _DMA_H
#include "sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,uint32_t par,uint32_t mar,uint16_t ndtr,uint32_t direction,uint8_t DataSize);//配置DMAx_CHx
void DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx);	//使能一次DMA传输

void DMA_send(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,\
                        uint32_t par,uint32_t mar,uint16_t ndtr,\
                        uint32_t direction,uint8_t DataSize);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //_DMA_H
