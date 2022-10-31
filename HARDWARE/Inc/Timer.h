/**
* @Author kokirika
* @Name TIMER
* @Date 2022-10-31
**/

#ifndef KOKIRIKA_TIMER_H
#define KOKIRIKA_TIMER_H

#include "sys.h"
#include <functional>

class Timer {
private:
    uint8_t timer_num{};
    TIM_TypeDef* Timx{};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure{};
    NVIC_InitTypeDef NVIC_InitStructure{};
    void RCC_init();
    static void Timer_extern_init();
public:
    Timer(TIM_TypeDef* TIMx,uint32_t arr,uint16_t psc,bool nvic= false);
    void init(TIM_TypeDef* TIMx,uint32_t arr,uint16_t psc,bool nvic= false);
    void set_NVIC(bool sata,uint8_t Preemption=1,uint8_t SubPriority=3);

    void (*extern_IRQ_link)(){};
    void upload_extern_fun(void(* fun)());
    std::function<void()> localfunxx;
    void Timer_extern_fun(std::function<void()> fun);

    static void Timer_extern_upset(uint8_t num);
};


#endif //KOKIRIKA_TIMER_H
