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

uint16_t Communication::find_address(const uint16_t *data) {
    return (uint16_t)(data - this->data_BUS.to_u16);
}

void Communication::data_set(float *data,float value) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii]  = data;
            this->valuex[ii] = value;
            this->float_falg[ii]= 1;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

void Communication::data_set(uint16_t *data, uint16_t value) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datau[ii]  = data;
            this->valueu[ii] = value;
            this->float_falg[ii]= 2;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

void Communication::data_set(uint16_t data, uint16_t value) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->address[ii]  = data;
            this->valueu[ii] = value;
            this->float_falg[ii]= 3;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

bool Communication::data_sync() {
    uint8_t ii=0;
    for(;ii<COM_queue_num;ii++){
        if(this->queue_flag[ii])
            break;
    }
    if(ii<COM_queue_num){
        this->queue_flag[ii]= false;
        if(this->float_falg[ii]==2)
            return this->modbus_16_send(this->find_address(this->datau[ii]),
                                 &this->valueu[ii],1)==modbus::modbus_success;
        else if(this->float_falg[ii]==3){
            return this->modbus_16_send(this->address[ii],
                                 &this->valueu[ii],1)==modbus::modbus_success;
        }
        else if(this->float_falg[ii]==3){
            union {
                float _f;
                uint16_t _u16[2];
            }temp{
                    ._f=this->valuex[ii],
            };
            return this->modbus_16_send(this->find_address(this->datax[ii]),
                                 temp._u16,2)==modbus::modbus_success;
        }
    }
    else {
        return this->modbus_03_send(0,sizeof(this->data_BUS)/2)==modbus::modbus_success;
    }
    return false;
}




