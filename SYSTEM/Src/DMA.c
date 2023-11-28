/**
* @Author kokirika
* @Name DMA
* @Date 2022-09-16
**/
#include "DMA.h"

void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,uint32_t par,uint32_t mar,uint16_t ndtr,uint32_t direction,uint8_t DataSize)
{
    DMA_InitTypeDef  DMA_InitStructure;

    if((u32)DMA_Streamx>=(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ��

    }else
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1ʱ��ʹ��
    }
    DMA_DeInit(DMA_Streamx);

    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//�ȴ�DMA������

    /* ���� DMA Stream */
    DMA_InitStructure.DMA_Channel = chx;  //ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA �洢�� ��ַ
    DMA_InitStructure.DMA_DIR = direction;//�洢��������ģʽ
    DMA_InitStructure.DMA_BufferSize = ndtr;//���ݴ�����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
    switch (DataSize) {
        case 8:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
            break;
        case 16:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ���:16λ
            break;
        case 32:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//�������ݳ���:32λ
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//�洢�����ݳ���:32λ
            break;
        default:
            DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
            DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
            break;
    }
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
    DMA_Init(DMA_Streamx, &DMA_InitStructure);//��ʼ��DMA Stream
}
//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//ndtr:���ݴ�����
void DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx)
{
    DMA_Cmd(DMA_Streamx, DISABLE);            //�ر�DMA����
    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����
    //DMA_SetCurrDataCounter(DMA_Streamx,ndtr);  //���ݴ�����
    DMA_Cmd(DMA_Streamx, ENABLE);             //����DMA����
}

void DMA_send(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,\
                uint32_t par,uint32_t mar,uint16_t ndtr,\
                uint32_t direction,uint8_t DataSize)
{
    DMA_Config(DMA_Streamx,chx,par,mar,ndtr,direction,DataSize);
    DMA_Enable(DMA_Streamx);
}


