/**
* @Author kokirika
* @Name OUT_IN_PUT
* @Date 2022-09-13
**/

#ifndef Kokirika_OUT_IN_PUT_H
#define Kokirika_OUT_IN_PUT_H

#include "GPIO.h"
#include "HARD_BASE.h"
#include <functional>

class _OutPut_ {
protected:
    _GPIO_ GPIO;
private:
    uint8_t default_mode{};
    uint8_t value{};
    bool setted;
public:
    _OutPut_(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo=LOW);
    explicit _OutPut_(uint8_t Pinx,uint8_t Hi_Lo=LOW);
    _OutPut_();
    ~_OutPut_()=default;
    void init();
    void init(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo);
    void init(uint8_t Pinx,uint8_t Hi_Lo);
    void set(uint8_t ON_OFF);
    void set_value(uint8_t Hi_Lo);
    void change();
    void flicker(uint8_t time);
    void flicker(uint8_t time,uint8_t interval,uint8_t num=2);

    uint8_t get() const;
};

class _InPut_ {
protected:
    _GPIO_ GPIO;
private:
    NVIC_InitTypeDef   NVIC_InitStructure{};
    EXTI_InitTypeDef   EXTI_InitStructure{};
    uint8_t Down_level{};
    uint8_t pin_num{};
    bool setted{};

    static char Interrupt_name[2];
    static char Interrupt_channel[2];
    static char* Interrupt_ret[2];
    static void extern_init();
public:
    _InPut_(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo=LOW);
    explicit _InPut_(uint8_t Pinx,uint8_t Hi_Lo=LOW);
    _InPut_();
    ~_InPut_()=default;
    void init(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo);
    void init(uint8_t Pinx,uint8_t Hi_Lo);
    void init();
    void set_EXTI();
    void setNVIC(uint8_t Priority=3,uint8_t SubPriority=3,bool EnAble=true);
    void setNVIC_ENABLE(bool EnAble=true) ;

    void (*extern_IRQ_link)(){};
    void upload_extern_fun(void(* fun)());
    void (*extern_IRQ_link_r)(uint8_t){};
    void upload_extern_fun(void(* fun)(uint8_t));
    std::function<void()> localfunxx;
    void upload_extern_fun(std::function<void()> fun);
    void upload_extern_fun(Call_Back *extx) const;

    static void extern_upset(uint8_t num);

    uint8_t get_pin_num() const;
    uint8_t get_level();
    bool get_state();
    bool get_NVIC_state() const;
};


#endif //Kokirika_OUT_IN_PUT_H
