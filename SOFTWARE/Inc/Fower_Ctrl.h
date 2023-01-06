/**
* @Author kokirika
* @Name FOWER_CTRL
* @Date 2022-12-26
**/

#ifndef KOKIRIKA_FOWER_CTRL_H
#define KOKIRIKA_FOWER_CTRL_H

#include "IIC.h"
#include "SMI.h"
#include "Timer.h"
#include "PID_BASE.h"
#include "PWM.h"

class Differential_pressure: private Software_IIC,public SMI{

public:
    Differential_pressure(uint8_t Pin_Scl,uint8_t Pin_Sda,float pmax,float pmin){
        Software_IIC::init(Pin_Scl,Pin_Sda);
        SMI::init(this,pmax, pmin);
    }
};
#define Fower_Data_num 5
class Fower_Ctrl :public PID_BASE{
private:
    float data_t[5][Fower_Data_num]{};
    uint8_t data_n[5]{};
    float FF_value{};
    bool en;
protected:
    Pressure_BASE *liuya;
    Pressure_BASE *jiya;
    Pressure_BASE *daqiya;
    Temperature_BASE *jiwen;
    Temperature_BASE *daqiwen;
    Kalman filter[6]{};
    PWM_H *CONTROLLER;
    uint8_t chx;
public:
    float cur{};
    float FLOW_RATE{};
    Fower_Ctrl(Pressure_BASE *liu,Pressure_BASE *ji,Pressure_BASE *daqi,
               Temperature_BASE *jit,Temperature_BASE *daqit);
    void config(PWM_H *CONTR,uint8_t ch);
    float LiuYa{},JiYa{},DaQiYa{},JiWen{},DaQiWen{};
    void data_upset();
    float calculation_hole_flow();
    float calculation_entrance_flow();
    void  FLOW_RATE_change(float value);
    void  TURN_ON();
    void  TURN_OFF();
    void  upset();

};


#endif //KOKIRIKA_FOWER_CTRL_H
