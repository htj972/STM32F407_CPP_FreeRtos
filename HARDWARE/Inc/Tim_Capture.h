/**
* @Author TIM_CAPTURE
* @Name TIM_CAPTURE
* @Date 2022-12-09
**/

#ifndef KOKIRIKA_TIM_CAPTURE_H
#define KOKIRIKA_TIM_CAPTURE_H

#include "Timer.h"
#include "GPIO.h"
#include "PID_BASE.h"

class Tim_Capture:public Timer, public Call_Back{
public:
    enum Capture_Frq:uint32_t{
        _1M = 1000000,
        _1K = 1000,
        _1Hz= 1,
    };
protected:
    _GPIO_ GPIOx{};
    uint32_t 	    CAPTURE_VAL{};
    uint32_t 	    CAPTURE_LEN{};
    uint32_t 	    CAPTURE_MAX_LEN{};
    bool            OVER_flag;
    uint32_t 	    CAPTURE_TABLE[4096]{};
private:
    uint8_t CHANNEL{};
    TIM_ICInitTypeDef  TIM_ICInitStructure{};
    void GPIO_AF_config();
    void default_GPIO(uint8_t channelx);
    void set_Callback();
    bool GPIO_CH1();
    bool GPIO_CH2();
    bool GPIO_CH3();
    bool GPIO_CH4();
public:
    Kalman filte{};
    Tim_Capture(TIM_TypeDef* TIMx,uint8_t channelx,GPIO_Pin Pinx,uint32_t fifo_size=1024,uint32_t FRQ=Capture_Frq::_1M);
    Tim_Capture(TIM_TypeDef* TIMx,uint8_t channelx,uint32_t fifo_size=1024,uint32_t FRQ=Capture_Frq::_1M);
    explicit Tim_Capture(TIM_TypeDef* TIMx,uint32_t FRQ=Capture_Frq::_1M);
    Tim_Capture()=default;
    ~Tim_Capture();
    void init(TIM_TypeDef *TIMx, uint32_t FRQ=Capture_Frq::_1M);
//    bool config(int ch,...);
    void config_Pin(uint8_t channelx,uint32_t fifo_size=1024);
    void config_Pin(uint8_t channelx,GPIO_Pin Pinx,uint32_t fifo_size=1024);
    float get_CAPTURE_fifo();
    uint32_t get_CAPTURE_VAL() const;

    void start();
    void stop();
    void set_enable(bool en);

    void Callback(int num,char ** gdata) override;

};


#endif //KOKIRIKA_TIM_CAPTURE_H
