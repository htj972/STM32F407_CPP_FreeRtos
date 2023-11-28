/**
* @Author kokirika
* @Name DMA
* @Date 2022-09-16
**/
#include "DMA.h"

void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,uint32_t par,uint32_t mar,uint16_t ndtr,uint32_t direction,uint8_t DataSize)
{
    DMA_InitTypeDef  DMA_InitStructure;

    if((u32)DMA_Streamx>=(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能

    }else
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能
    }
    DMA_DeInit(DMA_Streamx);

    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//等待DMA可配置

    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel = chx;  //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA 存储器 地址
    DMA_InitStructure.DMA_DIR = direction;//存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = ndtr;//数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
    switch (DataSize) {
        case 8:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
            break;
        case 16:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度:16位
            break;
        case 32:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//外设数据长度:32位
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//存储器数据长度:32位
            break;
        default:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
            break;
    }
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
    DMA_Init(DMA_Streamx, &DMA_InitStructure);//初始化DMA Stream
}
//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//ndtr:数据传输量
void DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx)
{
    DMA_Cmd(DMA_Streamx, DISABLE);            //关闭DMA传输
    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//确保DMA可以被设置
    //DMA_SetCurrDataCounter(DMA_Streamx,ndtr);  //数据传输量
    DMA_Cmd(DMA_Streamx, ENABLE);             //开启DMA传输
}

void DMA_send(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,\
                uint32_t par,uint32_t mar,uint16_t ndtr,\
                uint32_t direction,uint8_t DataSize)
{
    DMA_Config(DMA_Streamx,chx,par,mar,ndtr,direction,DataSize);
    DMA_Enable(DMA_Streamx);
}


