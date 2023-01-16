/**
* @Author kokirika
* @Name FOWER_CTRL
* @Date 2022-12-26
**/


#include <cmath>
#include "Fower_Ctrl.h"

Fower_Ctrl::Fower_Ctrl(Pressure_BASE *liu, Pressure_BASE *ji, Pressure_BASE *daqi,
                       Temperature_BASE *jit,Temperature_BASE *daqit): PID_BASE(),
                       liuya(liu),jiya(ji),daqiya(daqi),jiwen(jit),daqiwen(daqit){
    this->filter[0].init(50,1);//*liuya;
    this->filter[1].init(50,1);//*jiya;
    this->filter[2].init(50,1);//*daqiya;
    this->filter[3].init(10,1);//*jiwen;
    this->filter[4].init(10,1);//*daqiwen;
    this->filter[5].init(50,2);//*FF_value;
    for(auto & ii : this->data_t){
        for (float & jj : ii)
            jj=0;
    }
    for (uint8_t & ii : this->data_n)
        ii=0;

    PID_BASE::init(0.8,0.3,0.8);
}

float Fower_Ctrl::calculation_hole_flow() {
    float fbuf = LiuYa;

    float density_1=0.28689097f*(273.15f+JiWen)/(DaQiYa*1000+JiYa);

    if(density_1<0) {this->FF_value=fbuf;return 0;} //结果小于0，忽略
    if(fbuf < 2)        fbuf = 0;  //如果动压小于2Pa，忽略，防止漂移
    else
    {
        fbuf = (float )sqrt((double )(fbuf*density_1)) * FLOW_RATE;  //计算孔板处流量
    }
    FF_value=fbuf;
    return fbuf;
}

float Fower_Ctrl::calculation_entrance_flow() {
    //折算到入口流量
    return this->calculation_hole_flow() * (DaQiYa*1000 +JiYa) *
            (273.15f + DaQiWen) / (DaQiYa*1000) / (273.15f + JiWen);
}

void Fower_Ctrl::FLOW_RATE_change(float value) {
    FLOW_RATE=value/FF_value*FLOW_RATE;
}

void Fower_Ctrl::data_upset() {
    float data_g[5];
    data_g[0]=liuya->get_pres();
    data_g[1]=jiya->get_pres();
    data_g[2]=daqiya->get_pres();
    data_g[3]=jiwen->get_temp();
    data_g[4]=daqiwen->get_temp();

    for(uint8_t ii=0;ii<5;ii++){
//        if(data_g[ii]>0)
            this->data_t[ii][this->data_n[ii]++]=filter[ii].Filter(data_g[ii]);
    }

    for(uint8_t & ii : this->data_n) {
        if (ii >= Fower_Data_num)ii = 0;
    }

    float sum=0;
    for(float & ii : this->data_t[0]) {
        sum+=ii;
    }
    LiuYa=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[1]) {
        sum+=ii;
    }
    JiYa=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[2]) {
        sum+=ii;
    }
    DaQiYa=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[3]) {
        sum+=ii;
    }
    JiWen=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[4]) {
        sum+=ii;
    }
    DaQiWen=sum/=Fower_Data_num;
}

void Fower_Ctrl::upset() {
    float OUT;
    cur=FF_value;

    if(en){
        OUT=this->calculate(cur);
        if(OUT>100)OUT=100;
        else if(OUT<4)OUT=4;

        if(this->CONTROLLER!= nullptr)
            this->CONTROLLER->set_Duty_cycle(this->chx,OUT);
    } else{
        if(this->CONTROLLER!= nullptr) {
            OUT=this->calculate(0,cur);
            if(OUT>100)OUT=100;
            else if(OUT<0)OUT=0;
            this->CONTROLLER->set_Duty_cycle(this->chx,OUT);
        }
        cur=0;
        this->clear();
    }
}

void Fower_Ctrl::config(PWM_H *CONTR, uint8_t ch) {
    CONTROLLER=CONTR,
    chx=ch;
}

void Fower_Ctrl::TURN_ON() {
    if(!en)
        en= true;
}

void Fower_Ctrl::TURN_OFF() {
    if(en)
        en= false;
}



