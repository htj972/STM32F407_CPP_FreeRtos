/**
* @Author kokirika
* @Name 74HC165
* @Date 2026-01-17
**/

#ifndef STM32F407_CPP_FREERTOS_HC165_H
#define STM32F407_CPP_FREERTOS_HC165_H

#include "sys.h"
#include "Out_In_Put.h"

class HC165 {
private:
    _OutPut_ CE, CLK, SHLD;
    _InPut_  SO;
    uint8_t len=1;
    uint8_t (*HC_change)[8];
    uint8_t *Input_data=nullptr;
public:
    HC165(GPIO_Pin CE, GPIO_Pin CLK, GPIO_Pin SHLD, GPIO_Pin SO, uint8_t len=1);
    void init();
    void set_shift(const char data[][8]);
    void set_shift(uint8_t num,const char data[8]);
    void set_en(bool onoff);
    void clear();
    void upset();

    void Get_input(uint8_t *data);//获取当前输出状态
    bool Get_input_bit(uint8_t num);//获取当前输出状态某一位
    ~HC165();

};


#endif //STM32F407_CPP_FREERTOS_HC165_H
