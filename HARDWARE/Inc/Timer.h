/**
* @Author kokirika
* @Name TIMER
* @Date 2022-10-31
**/

#ifndef KOKIRIKA_TIMER_H
#define KOKIRIKA_TIMER_H

#include "sys.h"
#include <functional>
#include "HARD_BASE.h"


class Timer {
private:
    NVIC_InitTypeDef NVIC_InitStructure{};
    void RCC_init();
    static void extern_init();
    bool config_flag{};
protected:
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure{};
    uint8_t timer_num{};
    TIM_TypeDef* Timx{};
    static char Interrupt_data[2];
    static char Interrupt_name[2];
    static char Interrupt_channel[2];
    static char* Interrupt_ret[3];
public:
    Timer(TIM_TypeDef* TIMx,uint32_t arr,uint16_t psc,bool nvic= false);
    Timer();
    ~Timer()=default;
    void init(TIM_TypeDef* TIMx,uint32_t arr,uint16_t psc,bool nvic= false);
    void set_Cmd(bool sata);
    void set_NVIC(bool sata,uint8_t Preemption=1,uint8_t SubPriority=3);
    bool get_config() const;
    virtual uint8_t get_Timer_num();

    void (*extern_IRQ_link)(){};
    void upload_extern_fun(void(* fun)());
    void (*extern_IRQ_link_r)(uint8_t){};
    void upload_extern_fun(void(* fun)(uint8_t));
    std::function<void()> localfunxx;
    void upload_extern_fun(std::function<void()> fun);
    void upload_extern_fun(Call_Back *extx) const;

    void set_CCextern_fun(bool en) const;
    static void extern_upset(uint8_t num);
    static void extern_CC_upset(uint8_t num,uint8_t channel);
};


#endif //KOKIRIKA_TIMER_H
