/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/

#ifndef KOKIRIKA_COMMUNICATION_H
#define KOKIRIKA_COMMUNICATION_H


#include "RS485.h"
#include "Timer.h"
#include "modbus.h"

#define HARD_version 0.1

typedef struct K_USER_data{
    float version;          //版本号
    float air_temp{};         //采样口温度
    float stove_temp_r{};     //加热炉温度 读当前
    float stove_temp_w{};     //加热炉温度 设置目标
    float air_press{};        //大气压
    float gauge_temp{};       //计温
    float Flow_press{};       //流压
    float gauge_press{};      //计压
    float Valve{};            //阀
    float Flow_value_s{};     //流量 设置目标
    float Flow_value_r{};     //流量 读当前
    float Flow_value_w{};     //流量 设置当前 标定！
    float Flow_coefficient{}; //流量 读孔板系数
    float stove_P{};          //加热炉 P
    float stove_I{};          //加热炉 I
    float stove_D{};          //加热炉 D
    float Flow_P{};           //流量 P
    float Flow_I{};           //流量 I
    float Flow_D{};           //流量 D
    float stove_work{};       //加热炉工作
    float Flow_work{};        //流量控制工作
    float pump{};             //加热炉泵
}USER_data;

typedef union K_POWER_DATA_{
    USER_data to_float;
    uint16_t  to_u16[sizeof(USER_data)*2]{};
    uint8_t   to_u8t[sizeof(USER_data)*4];
    K_POWER_DATA_() {
        to_float.version=HARD_version;
        to_float.Flow_coefficient=1.0;
        to_float.stove_P=20;
        to_float.stove_I=1;
        to_float.stove_D=5;
        to_float.Flow_P=1;
        to_float.Flow_I=1;
        to_float.Flow_D=1;
    };
}K_POWER_DATA;

class Communication: private RS485, private Timer, public modbus {
public:
    Communication(USART_TypeDef* USARTx,uint8_t DE,TIM_TypeDef *TIMx, uint16_t frq,uint8_t PinTx,uint8_t PinRx);
    K_POWER_DATA data_BUS;
    void initial();
};




#endif //KOKIRIKA_COMMUNICATION_H
