/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq) {
    RS485::init(USARTx,DE,9600);
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
    this->set_id(1);//PH
    if (this->modbus_03_send(0, 6) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.water_pump_state = *data;
        this->env.fertilizer_pump_state = *(data+1);
        this->env.water_flow_clear = *(data+2);
        this->env.fertilizer_flow_clear = *(data+3);
        this->env.water_supply_reset = *(data+4);
        this->env.fertilizer_reset = *(data+5);
    }
}

string Communication::data_to_json() const {
    string buf;
    buf.append("{\"水泵状态\":"+to_string(this->env.water_pump_state)+",");
    buf.append("\"肥泵状态\":"+to_string(this->env.fertilizer_pump_state)+",");
    buf.append("\"水流量累计清零\":"+to_string(this->env.water_flow_clear)+",");
    buf.append("\"肥流量累计清零\":"+to_string(this->env.fertilizer_flow_clear)+",");
    buf.append("\"复位供水变频异常\":"+to_string(this->env.water_supply_reset)+",");
    buf.append("\"复位肥变频异常\":"+to_string(this->env.fertilizer_reset)+"}");
    return buf;
}
