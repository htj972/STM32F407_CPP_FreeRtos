/**
* @Author kokirika
* @Name Project
* @Date 2023-03-13
**/

#ifndef STM32F407_CPP_FREERTOS_TIMER_QUEUE_H
#define STM32F407_CPP_FREERTOS_TIMER_QUEUE_H

#include "Timer.h"

#define TIMER_QUEUE_MAX 10

class Timer_queue: public Timer,private Call_Back{
private:
    uint8_t timer_queue_num{};
    Call_Back *Ext[TIMER_QUEUE_MAX]{};
public:
    Timer_queue()=default;
    Timer_queue(TIM_TypeDef* TIMx,uint32_t FRQ);
    void init(TIM_TypeDef* TIMx,uint32_t FRQ);
    void upload_extern_fun(Call_Back *event);

    void Callback(int,char** data) override;
    uint8_t get_Timer_num() override;
};


#endif //STM32F407_CPP_FREERTOS_TIMER_QUEUE_H
