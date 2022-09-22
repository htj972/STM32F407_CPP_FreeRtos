/**
* @Author kokirika
* @Name SPI
* @Date 2022-09-17
**/

#ifndef Kokirika_SPI_H
#define Kokirika_SPI_H

#include "GPIO.h"

class SPI {
private:
    _GPIO_ SCK;
    _GPIO_ MISO;
    _GPIO_ MOSI;
    SPI_TypeDef* SPIx;
    SPI_InitTypeDef SPI_InitStructure;
    uint8_t config_flag=0;

    void default_config();

public:
    void init(SPI_TypeDef* SPI,uint16_t DataSize=SPI_DataSize_8b,uint8_t SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8);
    void config(GPIO_TypeDef *PORT_SCK,uint32_t Pin_SCK,\
                GPIO_TypeDef *PORT_MISO,uint32_t Pin_MISO,\
                GPIO_TypeDef *PORT_MOSI,uint32_t Pin_MOSI);
    void config(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI);
    void SetSpeed(uint8_t SPI_BaudRatePrescaler);
    uint16_t ReadWriteDATA(uint16_t TxData);


};


#endif //Kokirika_SPI_H
