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
    this->set_id(0x2);//盐分 电导率
    if (this->modbus_03_send(0, 2) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.soil_sa = (float) *data;// mg/L
        data++;
        this->env.soil_ec = (float) *data/100.0f;// mS/cm
    }
    this->set_id(0x01);//温湿度
    if (this->modbus_03_send(0, 3) == modbus::modbus_success) {
        uint16_t *data = this->data_BUS.to_u16;
        this->env.soil_N = (float) *data;// mg/kg
        data++;
        this->env.soil_P = (float) *data;
        data++;
        this->env.soil_K = (float) *data;
    }

}

string Communication::data_to_json() const {
    string buf;
    buf.append("{\"土壤盐分\":"+to_string(this->env.soil_sa));
    buf.append(",\"土壤电导率\":"+to_string(this->env.soil_ec));
    buf.append(",\"土壤氮\":"+to_string(this->env.soil_N));
    buf.append(",\"土壤磷\":"+to_string(this->env.soil_P));
    buf.append(",\"土壤钾\":"+to_string(this->env.soil_K));
    buf.append("}");
    return buf;
}
