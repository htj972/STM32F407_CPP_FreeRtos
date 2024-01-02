/**
* @Author kokirika
* @Name WH_COM
* @Date 2023-12-12
*/
#include "WH_COM.h"

WH_COM::WH_COM(USART_TypeDef *USARTx, TIM_TypeDef *TIMx, uint16_t frq) {
    _USART_::init(USARTx);
    Timer::init(TIMx,10000/frq,8400,true);
    WH_L101::init(this);
    this->initial();
    for (bool & ii : this->queue_flag) {
        ii= false;
    }
}

void WH_COM::initial() {
//    WH_L101::set_mode(4700,10);
    WH_L101::Link_UART_CALLback();
    WH_L101::Link_TIMER_CALLback(this);
    WH_L101::config(this->data_BUS.to_u16,sizeof(this->data_BUS)/2-1);
}

uint16_t WH_COM::find_address(const float *data) {
    return (uint16_t)(data - (float *)&this->data_BUS.to_float)*2;
}

uint16_t WH_COM::find_address(const uint16_t *data) {
    return (uint16_t)(data - this->data_BUS.to_u16);
}

void WH_COM::data_set(float *data,float value) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii]  = data;
            this->valuex[ii] = value;
            this->queue_flag[ii]= true;
            this->queue_flag_r[ii]= false;
            break;
        }
    }
}

void WH_COM::data_set(uint16_t *data,uint16_t value) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datau[ii]  = data;
            this->valueu[ii] = value;
            this->queue_flag[ii]= true;
            this->queue_flag_r[ii]= true;
            break;
        }
    }
}

bool WH_COM::data_sync() {
    bool ret = false;
    uint8_t ii=0;
    for(;ii<COM_queue_num;ii++){
        if(this->queue_flag[ii])
            break;
    }
    if(ii<COM_queue_num){
        if(!this->queue_flag_r[ii]) {
            union {
                float _f;
                uint16_t _u16[2];
            } temp{
                    ._f=this->valuex[ii],
            };
            if(0==this->modbus_16_send(this->find_address(this->datax[ii]),
                                 temp._u16, 2))
                ret = true;
            this->queue_flag[ii] = false;
        }
        else{
            if(0==this->modbus_16_send(this->find_address(this->datau[ii]),
                                 &this->valueu[ii],1))
                ret = true;
            this->queue_flag[ii] = false;
        }
    }
    else {
        if(0==this->modbus_03_send(0,sizeof(this->data_BUS)/2))
            ret = true;
    }
    return ret;
}