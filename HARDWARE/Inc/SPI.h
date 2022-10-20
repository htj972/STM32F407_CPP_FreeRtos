/**
* @Author kokirika
* @Name SPI
* @Date 2022-09-17
**/

#ifndef Kokirika_SPI_H
#define Kokirika_SPI_H

#include "GPIO.h"
#include "HARD_BASE.h"

class SPI: public HARD_BASE{

private:
    _GPIO_ SCK;
    _GPIO_ MISO;
    _GPIO_ MOSI;
    SPI_TypeDef* SPIx{};
    SPI_InitTypeDef SPI_InitStructure{};
    uint8_t config_flag=0;
    uint8_t DMA_Enable=OFF;
    DMA_Stream_TypeDef* DMAy_Streamx{};
    uint32_t DMA_FLAG{};
    uint32_t DMA_CHANNEL{};
    bool DMA_send_flag= false;

    void default_config();

public:
    explicit SPI(SPI_TypeDef* SPI,Queue mode =Queue::OWN_Queue,uint16_t DataSize=SPI_DataSize_8b,uint8_t SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8);
    ~SPI()=default;
    void init(SPI_TypeDef* SPI,Queue mode =Queue::OWN_Queue,uint16_t DataSize=SPI_DataSize_8b,uint8_t SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8);
    void config(GPIO_TypeDef *PORT_SCK,uint32_t Pin_SCK,\
                GPIO_TypeDef *PORT_MISO,uint32_t Pin_MISO,\
                GPIO_TypeDef *PORT_MOSI,uint32_t Pin_MOSI);
    void config(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI);
    void SetSpeed(uint8_t SPI_BaudRatePrescaler);
    uint16_t ReadWriteDATA(uint16_t TxData);

    void set_send_DMA(FunctionalState enable);
    void set_dma_streamx(DMA_Stream_TypeDef* DMAy_Stream);
    void DMA_WriteData(uint16_t *TxData,uint16_t len);

};


#endif //Kokirika_SPI_H
