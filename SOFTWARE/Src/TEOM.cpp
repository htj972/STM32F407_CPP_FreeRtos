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
//    //����������ȷ��  ͷ���ϵ磨�汾�ţ�
//    if(this->DATA.to_float.version-HARD_version!=0){
//        POWER_TEOM_DATA init_data;             //��ʵ��һ�������� ���췽�����ó�ʼ��ֵ
//        this->DATA=init_data;       //ת�����ݵ�ʹ�ýṹ��
//        this->write(0,init_data.to_u8t,sizeof(init_data));  //д������
//    }
}
