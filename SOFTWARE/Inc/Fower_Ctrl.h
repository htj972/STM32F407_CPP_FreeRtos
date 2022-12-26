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

class Differential_pressure: private Software_IIC,private Timer,public SMI,public Call_Back{
private:
    float press_table[255]{};
    uint8_t press_num{};
    float press_value{};
public:
    Differential_pressure(uint8_t Pin_Scl,uint8_t Pin_Sda,float pmax,float pmin,TIM_TypeDef *TIMx, uint16_t frq){
        Software_IIC::init(Pin_Scl,Pin_Sda);
        SMI::init(this,pmax, pmin);
        Timer::init(TIMx,10000/frq,8400,true);
        this->press_num=0;
        this->upload_extern_fun(this);
    }
    float get_sensor_pres() override{
        return press_value;
    }
    void Callback(int  ,char** gdata ) override{
        if(gdata[0][0]==Call_Back::Name::timer) {
            this->set_Cmd(false);
            float GT,GP;
            this->get_temp_pres_value(&GT,&GP);
            this->press_table[this->press_num++]=GP;
            if(this->press_num>=50)
            {
                float sum=0;
                for (int i = 0; i < 20; i++) {
                    sum+=this->press_table[i];
                }
                this->press_value=sum/50.0f;
            }
            this->set_Cmd(true);
        }
    };
};

class Fower_Ctrl {
private:
    float FLOW_RATE{};
protected:
    Pressure_BASE *liuya;
    Pressure_BASE *jiya;
    Pressure_BASE *daqiya;
    Temperature_BASE *jiwen;
    Temperature_BASE *daqiwen;
public:
    Fower_Ctrl(Pressure_BASE *liu,Pressure_BASE *ji,Pressure_BASE *daqi,
               Temperature_BASE *jit,Temperature_BASE *daqit):liuya(liu),
               jiya(ji),daqiya(daqi),jiwen(jit),daqiwen(daqit){

    }
    float calculation_hole_flow();
    float calculation_entrance_flow();
};


#endif //KOKIRIKA_FOWER_CTRL_H
