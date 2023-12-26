/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq) {
    RS485::init(USARTx,DE);
    RS485::config(GPIOD8,GPIOD9);
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
    this->set_id(0x19);//辐射
    if (this->modbus_03_send(2, 4) == modbus::modbus_success) {
        uint16_t *data = &this->data_BUS.to_u16[2];
        this->env.solar_rad = (float) *data;
        data+=3;
        this->env.light = (float) *data;
    }
    this->set_id(0x05);//温湿度
    if (this->modbus_03_send(0, 10) == modbus::modbus_success) {
        uint16_t *data = this->data_BUS.to_u16;
        this->env.humi = ((float) *data/ 10.0f);
        data++;
        this->env.temp = ((float) *data/ 10.0f);
        data+=3;
        this->env.pm25 = (float) *data;
        data+=5;
        this->env.pm10 = (float) *data;
    }
    this->set_id(0x0a);//温湿度
    if (this->modbus_03_send(6, 1) == modbus::modbus_success) {
        uint16_t *data = &this->data_BUS.to_u16[6];
        this->env.soil_ph = ((float) *data / 100.0f);
    }
    if (this->modbus_03_send(18, 3) == modbus::modbus_success) {
        uint16_t *data = &this->data_BUS.to_u16[18];
        this->env.soil_humi = ((float) *data / 10.0f);
        data++;
        this->env.soil_temp = ((float) *data / 10.0f);
        data++;
        this->env.soil_ec = ((float) *data / 10.0f);
    }
    if (this->modbus_03_send(30, 3) == modbus::modbus_success) {
        uint16_t *data = &this->data_BUS.to_u16[30];
        this->env.soil_N = (float) *data ;
        data++;
        this->env.soil_P = (float) *data ;
        data++;
        this->env.soil_K = (float) *data ;
    }
}

string Communication::data_to_json() const {
    string buf;
    buf.append("{\"空气温度\":"+to_string(this->env.temp));
    buf.append(",\"空气湿度\":"+to_string(this->env.humi));
    buf.append(",\"空气压力\":"+to_string(this->env.press));
    buf.append(",\"二氧化碳\":"+to_string(this->env.co2));
    buf.append(",\"光照强度\":"+to_string(this->env.light));
    buf.append(",\"PM2.5\":"+to_string(this->env.pm25));
    buf.append(",\"PM10\":"+to_string(this->env.pm10));
    buf.append(",\"太阳辐射\":"+to_string(this->env.solar_rad));
    buf.append(",\"土壤温度\":"+to_string(this->env.soil_temp));
    buf.append(",\"土壤湿度\":"+to_string(this->env.soil_humi));
    buf.append(",\"土壤电导率\":"+to_string(this->env.soil_ec));
    buf.append(",\"土壤氮\":"+to_string(this->env.soil_N));
    buf.append(",\"土壤磷\":"+to_string(this->env.soil_P));
    buf.append(",\"土壤钾\":"+to_string(this->env.soil_K));
    buf.append(",\"土壤PH\":"+to_string(this->env.soil_ph));
    buf.append("}");
    return buf;
}
