/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq) {
    RS485::init(USARTx,DE);
//    RS485::config(PinTx,PinRx);
    Timer::init(TIMx,10000/frq,8400,true);
    modbus::init(this,modbus::HOST);
    this->initial();
    for (bool & ii : this->queue_flag) {
        ii= false;
    }
}

void Communication::initial() {
    modbus::Link_UART_CALLback();
    modbus::Link_TIMER_CALLback(this);
    modbus::config(this->data_BUS.to_u16,sizeof(this->data_BUS)/2-1);

}

uint16_t Communication::find_address(const float *data) {
    return (uint16_t)(data - (float *)&this->data_BUS.to_float)*2;
}

void Communication::data_set(float *data,float value) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii]  = data;
            this->valuex[ii] = value;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

void Communication::data_sync() {
    uint8_t ii=0;
    for(;ii<COM_queue_num;ii++){
        if(this->queue_flag[ii])
            break;
    }
    if(ii<COM_queue_num){
        union {
            float _f;
            uint16_t _u16[2];
        }temp{
                ._f=this->valuex[ii],
        };
        this->modbus_16_send(this->find_address(this->datax[ii]),
                             temp._u16,2);
        this->queue_flag[ii]= false;
    }
    else {
        this->sensordata_sync();
    }
}

void Communication::sensordata_sync() {
    this->set_id(0x0B);//多合一传感器
    uint16_t data_last=0;
    if(this->modbus_03_send(0,28)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.humi=((float)*data/10.0f);//1
        data++;
        this->env.temp=((float)*data/10.0f);//2
        data++;
        this->env.soil_humi=((float)*data/10.0f);//3
        data++;
        this->env.soil_temp=((float)*data/10.0f);//4
        data++;
        this->env.pm25=((float)*data);//5
        data+=2;//空两个
        data_last=*data;
        data++;
        this->env.light=(float )((data_last<<16)+(*data));//9
        data++;
        this->env.pm10=((float)*data);//10
        data++;
        data_last=*data;
        data++;
        this->env.press=(float )((data_last<<16)+(*data));//12
        data++;
        data_last=*data;
        data++;
        this->env.PH=(float )((data_last<<16)+(*data));//14
        data++;
        this->env.rain=((float)*data/10.0f);//15
        data++;
        data_last=*data;
        data++;
        this->env.solar_rad=(float )((data_last<<16)+(*data));//17
        data+=4;//空四个
        this->env.soil_ec=((float)*data);//22
        data++;
        this->env.wind_dir=((float)*data);//23
        data++;
        this->env.wind_speed=((float)*data/10.0f);//24
        data+=2;//空两个
        this->env.soil_N=((float)*data);//26
        data++;
        this->env.soil_P=((float)*data);//27
        data++;
        this->env.soil_K=((float)*data);//28
    }
}

string Communication::data_to_json() const {
    string buf;


    return buf;
}

