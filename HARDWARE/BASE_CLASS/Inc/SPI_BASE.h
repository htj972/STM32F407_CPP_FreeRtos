/**
* @Author kokirika
* @Name SPI_BASE
* @Date 2022-10-28
**/

#ifndef KOKIRIKA_SPI_BASE_H
#define KOKIRIKA_SPI_BASE_H

#include "HARD_BASE.h"

class SPI_BASE:public HARD_BASE {
private:
public:
    virtual void SetSpeed(uint8_t SPI_BaudRatePrescaler);
    virtual uint16_t ReadWriteDATA(uint16_t TxData);
};


#endif //KOKIRIKA_SPI_BASE_H
