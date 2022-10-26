/**
* @Author kokirika
* @Name DS18B20
* @Date 2022-10-26
**/

#include "DS18B20.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"


DS18B20::DS18B20(uint8_t Pinx, uint16_t err_t) {
    this->config(Pinx, err_t);
}

DS18B20::DS18B20(GPIO_TypeDef *PORT, uint32_t Pinx, uint16_t err_t) {
    this->config(PORT,Pinx, err_t);
}

DS18B20::DS18B20() {
    this->config_f= false;
}

void DS18B20::init() {
    if(this->config_f)
        this->default_init();
}

void DS18B20::init(uint8_t Pinx, uint16_t err_t) {
    this->config(Pinx, err_t);
    this->default_init();
}

void DS18B20::init(GPIO_TypeDef *PORT_csl, uint32_t Pin_csl, uint16_t err_t) {
    this->config(PORT_csl, Pin_csl, err_t);
    this->default_init();
}

void DS18B20::Rst() {
    this->SET_OUT(LOW);
    delay_us(750);    //拉低750us
    this->SET_OUT(HIGH);
    delay_us(15);     //15US
}

uint8_t DS18B20::Read_Bit() {
    u8 data;
    this->SET_OUT(LOW);
    delay_us(2);
    this->SET_OUT(HIGH);
    this->SET_IN();
    delay_us(12);
    data=this->get_in();
    delay_us(50);
    return data;
}

void DS18B20::Write_Byte(uint8_t dat)
{
    uint8_t j;
    uint8_t testb;
    for (j=1;j<=8;j++)
    {
        testb=dat&0x01;
        dat=dat>>1;
        if (testb)
        {
            this->SET_OUT(LOW);
            delay_us(2);
            this->SET_OUT(HIGH);
            delay_us(60);
        }
        else
        {
            this->SET_OUT(LOW);
            delay_us(60);
            this->SET_OUT(HIGH);
            delay_us(2);
        }
    }
}

bool DS18B20::default_init() {
    this->Rst();
    return this->Check();
}

void DS18B20::Start()
{
    this->Rst();
    this->Check();
    this->Write_Byte(0xcc);// skip rom
    this->Write_Byte(0x44);// convert
}

uint8_t DS18B20::Read_Byte() {
    uint8_t i,j,dat;
    dat=0;
    for (i=1;i<=8;i++)
    {
        j=this->Read_Bit();
        dat=(j<<7)|(dat>>1);
    }
    return dat;
}

float DS18B20::get_sensor_temp() {
    uint8_t temp;
    uint8_t TL,TH;
    int tem;
    taskENTER_CRITICAL();
    this->Start();
    this->Rst();
    this->Check();
    this->Write_Byte(0xcc);
    this->Write_Byte(0xbe);
    TL=this->Read_Byte();
    TH=this->Read_Byte();
    taskEXIT_CRITICAL();
    if(TH>7)
    {
        TH=~TH;
        TL=~TL;
        temp=0;//温度为负
    }else temp=1;//温度为正
    tem=(TH<<8)+TL; //获得高八位
    if(temp)return (float )tem*0.0625f; //返回温度值
    else return -(float )tem*0.0625f;
}
