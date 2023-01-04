/**
* @Author kokirika
* @Name TEOM
* @Date 2022-12-27
**/

#include "TEOM.h"


void PDATA_Storage::inital() {
    FM24Cxx ::init(this);
}

TEOM::TEOM(GPIO_Pin ENx,TIM_TypeDef* TIMx,uint8_t channel) {
    this->init(ENx,TIMx,channel);
}

void TEOM::init(GPIO_Pin ENx, TIM_TypeDef *TIMx, uint8_t channel) {
    this->EN.init(ENx,HIGH);
    this->TimxC.init(TIMx);
    this->TimxC.config_Pin(channel,4000);
    this->TimxC.filte.init(50,2);
}

void TEOM::turn_on() {
    this->EN.set(ON);
    this->TimxC.start();
}

void TEOM::turn_off() {
    this->EN.set(OFF);
    this->TimxC.stop();
}

float TEOM::get_frq() {
    static uint8_t  frqnum=0;
    static uint16_t Tt=65535-500;
    static bool ovoer_f= false;
    Tt++;
    if((Tt>20)&&(Tt<50))
    {
        Tt=0;
        this->frqdata[frqnum++]=1.0f/this->TimxC.get_CAPTURE_fifo()*1000*1000;
    }
    else if(Tt>=50)
        this->frqdata[frqnum++]=1.0f/this->TimxC.get_CAPTURE_fifo()*1000*1000;
    if(frqnum>=120){
        frqnum=0;
        ovoer_f= true;
    }
    float frq_sum=0;
    if(ovoer_f) {
        for (uint16_t ii = 0; ii < 120; ii++) {
            frq_sum += this->frqdata[ii];
        }
        frq_sum /= 120.0f;
    }
    else {
        for (uint16_t ii = 0; ii < frqnum; ii++) {
            frq_sum += this->frqdata[ii];
        }
        frq_sum /= 120.0f;
    }
    return frq_sum;
}

TEOM_TEMP::TEOM_TEMP(SPI_TypeDef *SPI, GPIO_Pin CS1, GPIO_Pin CS2, GPIO_Pin CS3, \
                    GPIO_Pin CS4, GPIO_Pin CS5,TIM_TypeDef* TIMx,uint32_t FRQ) {
    SPI::init(SPI1);
    PWM_H::init(TIMx,FRQ);
    this->CS_P[0]=CS1;
    this->CS_P[1]=CS2;
    this->CS_P[2]=CS3;
    this->CS_P[3]=CS4;
    this->CS_P[4]=CS5;
}

void TEOM_TEMP::initial() {
    for(uint8_t ii=0;ii<5;ii++) {
        this->temp_sensor[ii].init(this, CS_P[ii]);
        this->temp_sensor[ii].config(MAX31865::MODE::PT1000,200);
    }
    this->CTRLT[0].init(&this->temp_sensor[0],this,4);//管前
    this->CTRLT[1].init(&this->temp_sensor[1],nullptr,0);//管后  --查看
    this->CTRLT[2].init(&this->temp_sensor[2],this,3);//管后 -- 膜温
    this->CTRLT[3].init(&this->temp_sensor[3],this,1);//上盖
    this->CTRLT[4].init(&this->temp_sensor[4],this,2);//腔温

    this->CTRLT[0].config_kalman(0.1,0.034);
    this->CTRLT[1].config_kalman(0.1,0.034);
    this->CTRLT[2].config_kalman(0.1,0.034);
    this->CTRLT[3].config_kalman(0.1,0.034);
    this->CTRLT[4].config_kalman(0.1,0.034);

    this->CTRLT[0].config_PID(45,0.2,300);//管前
    this->CTRLT[2].config_PID(45,0.5,350);//管后 -- 膜温
    this->CTRLT[3].config_PID(45,0.15,200);//上盖
    this->CTRLT[4].config_PID(45,0.15,500);//腔温

    PWM_H::config(1,2,3,4);

}

void TEOM_Machine::Link_PRE(pressure_dif *Pre_linkx) {
    this->Pre_link=Pre_linkx;
}

void TEOM_Machine::inital() {
    PDATA_Storage::inital();
    this->read(0,this->DATA.to_u8t,sizeof(DATA));
    //检验数据正确性  头次上电（版本号）
//    if((this->DATA.to_float.version-HARD_version!=0)||(this->DATA.to_float.dis_light==0))
    {
        POWER_TEOM_DATA init_data;             //新实体一个新数据 构造方法放置初始数值
        this->DATA=init_data;       //转移数据到使用结构体
        this->write(0,init_data.to_u8t,sizeof(init_data));  //写入数据

    }
}

void TEOM_Machine::data_save(float *data,float value) {
    *data=value;
    this->write(0,this->DATA.to_u8t,sizeof(DATA));
}






