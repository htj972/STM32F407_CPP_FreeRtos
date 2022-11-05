/**
* @Author kokirika
* @Name TEMP_CTRL
* @Date 2022-11-02
**/

#include "Temp_ctrl.h"

Temp_ctrl::Temp_ctrl(Temperature_BASE *sensor, PWM_BASE *ctrl,uint8_t ch) {
    this->SENSOR=sensor;
    this->CONTROLLER=ctrl;
    this->chx=ch;
}

void Temp_ctrl::init(Temperature_BASE *sensor, PWM_BASE *ctrl,uint8_t ch) {
    this->SENSOR=sensor;
    this->CONTROLLER=ctrl;
    this->chx=ch;

    this->PID.init(5,0.1,10);
}

void Temp_ctrl::init() {
    this->PID.init(5,0.1,10);
}

void Temp_ctrl::set_target(float tar) {
    this->PID.set_target(tar);
}

void Temp_ctrl::PID_ENABLE_Range(float Range) {
    this->range=Range;
}

void Temp_ctrl::upset() {
    float OUT;
    float tar=this->PID.get_target();
    float cur=this->SENSOR->get_temp();
    if((this->range==0)||
        ((this->range!=0)&&(tar+this->range>cur)&&(tar-this->range<cur)))
    {
        OUT=this->PID.calculate(cur);
        if(OUT>100)OUT=100;
        else if(OUT<0)OUT=0;
    }
    else
    {
        if(cur<tar)OUT=100;
        else       OUT=0;
        this->PID.clear();
    }
    this->CONTROLLER->set_Duty_cycle(this->chx,OUT);
}






