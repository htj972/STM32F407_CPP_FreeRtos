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
    this->set_id(0x19);//����
    if (this->modbus_03_send(2, 4) == modbus::modbus_success) {
        uint16_t *data = &this->data_BUS.to_u16[2];
        this->env.solar_rad = (float) *data;
        data+=3;
        this->env.light = (float) *data;
    }
    this->set_id(0x05);//��ʪ��
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
    this->set_id(0x0a);//��ʪ��
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
    buf.append("{\"�����¶�\":"+to_string(this->env.temp));
    buf.append(",\"����ʪ��\":"+to_string(this->env.humi));
    buf.append(",\"����ѹ��\":"+to_string(this->env.press));
    buf.append(",\"������̼\":"+to_string(this->env.co2));
    buf.append(",\"����ǿ��\":"+to_string(this->env.light));
    buf.append(",\"PM2.5\":"+to_string(this->env.pm25));
    buf.append(",\"PM10\":"+to_string(this->env.pm10));
    buf.append(",\"̫������\":"+to_string(this->env.solar_rad));
    buf.append(",\"�����¶�\":"+to_string(this->env.soil_temp));
    buf.append(",\"����ʪ��\":"+to_string(this->env.soil_humi));
    buf.append(",\"�����絼��\":"+to_string(this->env.soil_ec));
    buf.append(",\"������\":"+to_string(this->env.soil_N));
    buf.append(",\"������\":"+to_string(this->env.soil_P));
    buf.append(",\"������\":"+to_string(this->env.soil_K));
    buf.append(",\"����PH\":"+to_string(this->env.soil_ph));
    buf.append("}");
    return buf;
}
