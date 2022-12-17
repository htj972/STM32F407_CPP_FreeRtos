/**
* @Author kokirika
* @Name PRETREATMENT
* @Date 2022-12-17
**/

#include "pretreatment.h"

pretreatment::pretreatment(SPI_BASE *SPIx, uint8_t CSpin, TIM_TypeDef *TIMx,
                           uint32_t FRQ, uint8_t ch):SCpinx(CSpin){
    PWM_H::init(TIMx,FRQ);
    Temp_ctrl::init(this,this,ch);
    MAX31865::spix=SPIx;
}

void pretreatment::initial() {
    MAX31865::init(MAX31865::spix,SCpinx);
    MAX31865::config(MAX31865::PT100,3900);
    PWM_H::config(chx);
    Temp_ctrl::PID.init(20,1,5);
}
