/**
* @Author kokirika
* @Name PWM
* @Date 2022-11-01
**/

#ifndef KOKIRIAK_PWM_H
#define KOKIRIAK_PWM_H

#include "Timer.h"
#include "PWM_BASE.h"
#include "GPIO.h"

class PWM_H :public Timer,public PWM_BASE{
private:
    TIM_OCInitTypeDef  TIM_OCInitStructure{};
    _GPIO_ GPIOx_CH[4];
    bool GPIO_init(uint8_t num);
    bool GPIO_CH1();
    bool GPIO_CH2();
    bool GPIO_CH3();
    bool GPIO_CH4();
    void OCinit(uint8_t num);
    void GPIO_AF_config(uint8_t channel);
public:
    PWM_H(TIM_TypeDef* TIMx,uint32_t FRQ);
    PWM_H(TIM_TypeDef* TIMx);
    PWM_H()=default;
    void init(TIM_TypeDef* TIMx,uint32_t FRQ);
    bool config(int ch,...);
    void config_Pin(PWM_BASE::channel channelx,uint8_t Pinx);
    void config_Pin(PWM_BASE::channel channelx,GPIO_TypeDef* PORTx,uint32_t Pinx);

    void set_channelx_ccr(uint8_t num,uint32_t ccr) override;
    void set_channel1_ccr(uint32_t ccr);
    void set_channel2_ccr(uint32_t ccr);
    void set_channel3_ccr(uint32_t ccr);
    void set_channel4_ccr(uint32_t ccr);
};


#endif //KOKIRIAK_PWM_H
