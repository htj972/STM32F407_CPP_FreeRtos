/**
* @Author kokirika
* @Name DHTXX
* @Date 2022-10-25
**/

#include "DHTxx.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"


DHTxx::DHTxx(uint8_t Pinx, uint16_t err_t) {
    this->config(Pinx, err_t);
}

DHTxx::DHTxx(GPIO_TypeDef *PORT,uint32_t Pinx,uint16_t err_t)
{
    this->config(PORT,Pinx, err_t);
}

DHTxx::DHTxx() {
    this->config_f= false;
}

void DHTxx::init() {
    if(this->config_f)
        this->default_init();
}

void DHTxx::init(uint8_t Pinx, uint16_t err_t) {
    this->config(Pinx, err_t);
    this->default_init();
}

void DHTxx::init(GPIO_TypeDef *PORT_csl, uint32_t Pin_csl, uint16_t err_t) {
    this->config(PORT_csl, Pin_csl, err_t);
    this->default_init();
}

void DHTxx::Rst()
{
    this->SET_OUT(LOW);
    delay_xms(20);
    this->SET_OUT(HIGH);
    delay_us(30);
}

uint8_t DHTxx::Read_Bit()
{
    uint8_t retry=0;
    while(this->get_in()&&this->check_err(retry))//等待变为低电平
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!this->get_in()&&this->check_err(retry))//等待变高电平
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);//等待40us
    return this->get_in();
}

uint8_t DHTxx::Read_Byte()
{
    uint8_t i,dat;
    dat=0;
    for (i=0;i<8;i++)
    {
        dat<<=1;
        dat|=this->Read_Bit();
    }
    return dat;
}

bool DHTxx::default_init()
{
    this->Rst();
    return this->Check();
}

bool DHTxx::get_sensor_temp_humi(float *Temp, float *Humi) {
    uint8_t buf[5];
    bool ret = true;
    taskENTER_CRITICAL();
    this->Rst();
    if(this->Check())
    {
        for(unsigned char & i : buf)
        {
            i=this->Read_Byte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            *Humi=buf[0];
            *Temp=buf[2];
        }
    }else ret = false;
    taskEXIT_CRITICAL();
    return ret;
}

float DHTxx::get_sensor_temp() {
    uint8_t buf[5];
    float Temp=0;
    taskENTER_CRITICAL();
    this->Rst();
    if(this->Check())
    {
        for(unsigned char & i : buf)
        {
            i=this->Read_Byte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            Temp=buf[2];
        }
    }
    taskEXIT_CRITICAL();
    return Temp;
}

float DHTxx::get_sensor_humi() {
    uint8_t buf[5];
    float Humi=0;
    taskENTER_CRITICAL();
    this->Rst();
    if(this->Check())
    {
        for(unsigned char & i : buf)
        {
            i=this->Read_Byte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            Humi=buf[0];
        }
    }
    taskEXIT_CRITICAL();
    return Humi;
}





