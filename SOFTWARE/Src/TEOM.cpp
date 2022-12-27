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
    this->TimxC.config_Pin(channel);
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
    uint32_t T=this->TimxC.get_CAPTURE_fifo();
    return 1.0f/(float)T*1000*1000;
}

void TEOM_Machine::inital() {
    PDATA_Storage::inital();
//    this->read(0,this->DATA.to_u8t,sizeof(DATA));
//    //检验数据正确性  头次上电（版本号）
//    if(this->DATA.to_float.version-HARD_version!=0){
//        POWER_TEOM_DATA init_data;             //新实体一个新数据 构造方法放置初始数值
//        this->DATA=init_data;       //转移数据到使用结构体
//        this->write(0,init_data.to_u8t,sizeof(init_data));  //写入数据
//    }
}
