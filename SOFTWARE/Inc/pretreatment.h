/**
* @Author kokirika
* @Name PRETREATMENT
* @Date 2022-12-17
**/

#ifndef KOKIRIKA_PRETREATMENT_H
#define KOKIRIKA_PRETREATMENT_H


#include "Temp_ctrl.h"
#include "MAX31865.h"
#include "PWM.h"

class pretreatment: public Temp_ctrl,public MAX31865,private PWM_H{
private:
    uint8_t SCpinx{};
public:
    pretreatment(SPI_BASE *SPIx,uint8_t CSpin,TIM_TypeDef *TIMx, uint32_t FRQ,uint8_t ch);
    void initial();
};


#endif //KOKIRIKA_PRETREATMENT_H
