/**
* @Author kokirika
* @Name PRETREATMENT
* @Date 2022-12-17
**/

#include "pretreatment.h"

pretreatment::pretreatment(SPI_BASE *SPIx, uint8_t CSpin,PWM_H *PWM,uint8_t ch,GPIO_Pin BENG,GPIO_Pin FA):SCpinx(CSpin){
    this->PWMx=PWM;
    Temp_ctrl::init(this,this->PWMx,ch);
    this->spix=SPIx;
    this->en= false;
    this->BENGx.init(BENG,HIGH);
    this->FAx.init(FA,HIGH);
    this->_05s_back_times=0;
}

void pretreatment::initial() {
    MAX31865::init(this->spix,SCpinx);
    MAX31865::config(MAX31865::PT100,3900);
    Temp_ctrl::config_PID(5,0.02,5);
    Temp_ctrl::config_kalman(0.1,0.05);
    Temp_ctrl::set_target(120);
}

void pretreatment::upset() {
    Temp_ctrl::upset(this->en);
    this->_05s_back_times++;
    if(this->_05s_back_times==60*2)
    {
        if(this->en)
            this->BENGx.set(ON);
        this->_05s_back_times=0;
    }
    else if(this->_05s_back_times==5*2)
    {
        if(this->en)
            this->BENGx.set(OFF);
        this->FAx.set(OFF);
    }
}

void pretreatment::turn_ON() {
    if(!this->en) {
        this->en = true;
        this->_05s_back_times=0;
    }
}

void pretreatment::turn_OFF() {
    if(this->en) {
        this->en = false;
        this->FAx.set(ON);
        this->_05s_back_times=0;
    }
}
