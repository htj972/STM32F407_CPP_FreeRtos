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
#include "Out_In_Put.h"

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

class PWM_S :public Timer,public PWM_BASE,public Call_Back{
private:
    class PWM_GPIO{
    public:
        _OutPut_ GPIOx;
        uint16_t ccr{};
        uint16_t arr{};
        bool en;
    }GPIOx_CH[4];


//    _OutPut_ GPIOx_CH[4];
//    uint16_t ccr[4]{};
//    uint16_t arr[4]{};
public:
    PWM_S(TIM_TypeDef* TIMx,uint32_t FRQ);
    PWM_S()=default;
    void init(TIM_TypeDef* TIMx,uint32_t FRQ);
    bool config(GPIO_Pin Pin1,uint8_t Pin2=0,uint8_t Pin3=0,uint8_t Pin4=0);
    void config_Pin(uint8_t channelx,uint8_t Pinx);
    void config_Pin(uint8_t channelx,GPIO_TypeDef* PORTx,uint8_t Pinx);

    void set_channelx_ccr(uint8_t num,uint32_t ccr) override;
    void set_channel1_ccr(uint32_t ccr);
    void set_channel2_ccr(uint32_t ccr);
    void set_channel3_ccr(uint32_t ccr);
    void set_channel4_ccr(uint32_t ccr);

    void Callback(int,char** data) override;
};


#endif //KOKIRIAK_PWM_H
