/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq) {
    RS485::init(USARTx,DE,4800);
    RS485::config(GPIOD8,GPIOD9);
    RS485::set_delay_times(2);
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
    this->set_id(18);//PH
    if (this->modbus_03_send(0, 1) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.soil_PH = (float)*data/10.0f;
    }
    this->set_id(3);//五合一
    if (this->modbus_03_send(0, 5) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.soil_Humi = (float)*data/10.0f;
        data++;
        this->env.soil_Temp = (float)*data/10.0f;
        data++;
        this->env.soil_EC = (float)*data;
        data++;
        this->env.soil_Sa = (float)*data;
        data++;
        this->env.soil_TDS = (float)*data;
    }
    this->set_id(2);//风向
    if (this->modbus_03_send(0, 1) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.wind_dir = *data;
    }
    this->set_id(1);//风速
    if (this->modbus_03_send(0, 1) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.wind_speed = (float)*data/10.0f;
    }
    this->set_id(8);//紫外辐射
    if (this->modbus_03_send(0, 1) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.UV = (float)*data/100.0f;
    }
    this->set_id(11);//四合一
    if (this->modbus_03_send(500, 8) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.air_Humi = (float)*data/10.0f;
        data++;
        this->env.air_Temp = (float)*data/10.0f;
        data++;
        this->env.air_ZS = (float)*data/10.0f;
        data+=4;
        uint16_t lastdata = *data;
        data++;
        this->env.light = float ((lastdata<<16)+*data);
    }
    this->set_id(28);//土壤氮磷钾
    if (this->modbus_03_send(30, 3) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.soil_N = (float)*data;
        data++;
        this->env.soil_P = (float)*data;
        data++;
        this->env.soil_K = (float)*data;
    }

}

string Communication::data_to_json() const {
    string buf;
    buf.append("{\"土壤PH\":"+to_string(this->env.soil_PH));
    buf.append(",\"土壤温度\":"+to_string(this->env.soil_Temp));
    buf.append(",\"土壤湿度\":"+to_string(this->env.soil_Humi));
    buf.append(",\"土壤EC\":"+to_string(this->env.soil_EC));
    buf.append(",\"土壤盐分\":"+to_string(this->env.soil_Sa));
    buf.append(",\"土壤TDS\":"+to_string(this->env.soil_TDS));
    buf.append(",\"风向\":");
    switch (this->env.wind_dir) {
        case 0:buf.append("\"北\"");break;
        case 1:buf.append("\"东北\"");break;
        case 2:buf.append("\"东\"");break;
        case 3:buf.append("\"东南\"");break;
        case 4:buf.append("\"南\"");break;
        case 5:buf.append("\"西南\"");break;
        case 6:buf.append("\"西\"");break;
        case 7:buf.append("\"西北\"");break;
    }
    buf.append(",\"风速\":"+to_string(this->env.wind_speed));
    buf.append(",\"紫外辐射\":"+to_string(this->env.UV));
    buf.append(",\"空气温度\":"+to_string(this->env.air_Temp));
    buf.append(",\"空气湿度\":"+to_string(this->env.air_Humi));
    buf.append(",\"空气噪声\":"+to_string(this->env.air_ZS));
    buf.append(",\"光照强度\":"+to_string(this->env.light));
    buf.append(",\"土壤氮\":"+to_string(this->env.soil_N));
    buf.append(",\"土壤磷\":"+to_string(this->env.soil_P));
    buf.append(",\"土壤钾\":"+to_string(this->env.soil_K));
    buf.append("}");
    return buf;
}
