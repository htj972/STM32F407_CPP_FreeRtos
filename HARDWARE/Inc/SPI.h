/**
* @Author kokirika
* @Name SPI
* @Date 2022-09-17
**/

#ifndef Kokirika_SPI_H
#define Kokirika_SPI_H

#include "GPIO.h"
#include "SPI_BASE.h"

class SPI: public SPI_BASE{
private:
    SPI_TypeDef* SPIx{};
    SPI_InitTypeDef SPI_InitStructure{};
    uint8_t config_flag=0;
    uint8_t DMA_Enable=OFF;
    DMA_Stream_TypeDef* DMAy_Streamx{};
    uint32_t DMA_FLAG{};
    uint32_t DMA_CHANNEL{};
    bool DMA_send_flag= false;

    void default_config();

protected:
    _GPIO_ SCK;
    _GPIO_ MISO;
    _GPIO_ MOSI;

public:
    explicit SPI(SPI_TypeDef* SPI,Queue mode =Queue::OWN_Queue,uint16_t DataSize=SPI_DataSize_8b,uint8_t SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8);
    SPI();
    ~SPI()=default;
    void init(SPI_TypeDef* SPI,Queue mode =Queue::OWN_Queue,uint16_t DataSize=SPI_DataSize_8b,uint8_t SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8);
    void init(Queue mode =Queue::OWN_Queue);
    void config(GPIO_TypeDef *PORT_SCK,uint32_t Pin_SCK,\
                GPIO_TypeDef *PORT_MISO,uint32_t Pin_MISO,\
                GPIO_TypeDef *PORT_MOSI,uint32_t Pin_MOSI);
    void config(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI);
    void SetSpeed(uint8_t SPI_BaudRatePrescaler) override;
    uint16_t ReadWriteDATA(uint16_t TxData) override;

    void set_send_DMA(FunctionalState enable);
    void set_dma_streamx(DMA_Stream_TypeDef* DMAy_Stream);
    void DMA_WriteData(uint16_t *TxData,uint16_t len);
};

class SPI_S: public SPI_BASE{
public:
    enum CP:uint8_t{
        OL_0_HA_0   =   0,
        OL_0_HA_1   =   1,
        OL_1_HA_0   =   2,
        OL_1_HA_1   =   3,
    };
private:
    _GPIO_ SCK;
    _GPIO_ MISO;
    _GPIO_ MOSI;
    uint8_t BaudRate{};
    uint8_t RW_mode{};
    uint8_t SOFT_SPI_RW_MODE0( uint8_t write_dat);/* CPOL = 0, CPHA = 0, MSB first */
    uint8_t SOFT_SPI_RW_MODE1( uint8_t write_dat);/* CPOL = 0£¬CPHA = 1, MSB first */
    uint8_t SOFT_SPI_RW_MODE2( uint8_t write_dat);/* CPOL = 1£¬CPHA = 0, MSB first */
    uint8_t SOFT_SPI_RW_MODE3( uint8_t write_dat );/* CPOL = 1,CPHA = 1, MSB first */
public:
    SPI_S(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI,Queue mode =Queue::OWN_Queue);
    SPI_S(GPIO_TypeDef *PORT_SCK,uint32_t Pin_SCK,\
                GPIO_TypeDef *PORT_MISO,uint32_t Pin_MISO,\
                GPIO_TypeDef *PORT_MOSI,uint32_t Pin_MOSI,Queue mode =Queue::OWN_Queue);
    ~SPI_S()=default;
    void init(Queue mode =Queue::OWN_Queue);
    void init(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI,Queue mode =Queue::OWN_Queue);
    void init(GPIO_TypeDef *PORT_SCK,uint32_t Pin_SCK,\
                GPIO_TypeDef *PORT_MISO,uint32_t Pin_MISO,\
                GPIO_TypeDef *PORT_MOSI,uint32_t Pin_MOSI,Queue mode =Queue::OWN_Queue);
    void config(CP RWmode);
    void SetSpeed(uint8_t SPI_BaudRatePrescaler) override;
    uint16_t ReadWriteDATA(uint16_t TxData) override;

};


#endif //Kokirika_SPI_H
