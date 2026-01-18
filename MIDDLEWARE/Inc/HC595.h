/**
* @Author kokirika
* @Name 74HC595
* @Date 2026-01-17
**/

#ifndef STM32F407_CPP_FREERTOS_HC595_H
#define STM32F407_CPP_FREERTOS_HC595_H

#include "sys.h"
#include "Out_In_Put.h"

class HC595 {
private:
    _OutPut_ en, clk, rck, dio, clr;
    uint8_t len=1;
    uint8_t (*HC_change)[8];
    uint8_t *Output_data=nullptr;
public:
    HC595(GPIO_Pin en, GPIO_Pin clk, GPIO_Pin rck, GPIO_Pin dio, GPIO_Pin clr,uint8_t len=1);
    void init();
    void set_shift(const char data[][8]);
    void set_shift(uint8_t num,const char data[8]);
    void set_en(bool onoff);
    void clear(bool dis=true);
    void upset();
    void Set_Hex(const uint8_t *data);//发送hex数据
    void Set_output(uint8_t num,uint8_t onoff);//发送bin数据
    void Set_on(uint8_t num);
    void Set_off(uint8_t num);
    void Toggle(uint8_t num);
    void Get_output(uint8_t *data);//获取当前输出状态
    bool Get_output_bit(uint8_t num);//获取当前输出状态某一位
    ~HC595();

};


#endif //STM32F407_CPP_FREERTOS_HC595_H
