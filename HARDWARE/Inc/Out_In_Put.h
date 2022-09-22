/**
* @Author kokirika
* @Name OUT_IN_PUT
* @Date 2022-09-13
**/

#ifndef Kokirika_OUT_IN_PUT_H
#define Kokirika_OUT_IN_PUT_H

#include "GPIO.h"

class _OutPut_ {
private:
    _GPIO_ GPIO;
    uint8_t default_mode;
    uint8_t value;
public:
    void init(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo);
    void init(uint8_t Pinx,uint8_t Hi_Lo);
    void set(uint8_t ON_OFF);
    void set_value(uint8_t Hi_Lo);
    void change();
    uint8_t get() const;
};

class _InPut_ {
private:
    _GPIO_ GPIO;
    NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;
    uint8_t Down_level;
    uint8_t pin_num;
public:
    void init(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo);
    void init(uint8_t Pinx,uint8_t Hi_Lo);
    void set_EXTI();
    void setNVIC(uint8_t Priority=3,uint8_t SubPriority=3,bool EnAble=true);
    void setNVIC_ENABLE(bool EnAble=true) ;

    static void upload_extern_fun(void(* fun)(uint8_t channel));
    static void send_to_fifo(uint8_t channel);

    uint8_t get_level();
    bool get_state();
    bool get_NVIC_state() const;
};


#endif //Kokirika_OUT_IN_PUT_H
