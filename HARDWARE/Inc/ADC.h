/**
* @Author kokirika
* @Name ADC
* @Date 2022-09-22
**/

#ifndef Kokirika_ADC_H
#define Kokirika_ADC_H

#include "GPIO.h"

class _ADC_ {
private:
    _GPIO_  GPIO;
    ADC_TypeDef* ADCx;
    uint8_t channel;
    uint16_t delay_time= 5;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef       ADC_InitStructure;
    void default_config();
    void RCC_config();
    void delay() const;

public:
    void init(ADC_TypeDef* ADCX,uint8_t channel);
    uint16_t get_value();
    void set_delay_time(uint16_t delay);
    uint16_t Get_Adc_Average(uint16_t times=5);
};


#endif //Kokirika_ADC_H
