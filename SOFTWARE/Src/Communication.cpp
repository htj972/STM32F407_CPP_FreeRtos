/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

#include <utility>

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq) {
    RS485::init(USARTx,DE,9600);
    RS485::config(GPIOD8,GPIOD9);
    RS485::set_delay_times(2);
    Timer::init(TIMx,10000/frq,8400,false);
    modbus::init(this,modbus::HOST);
    //this->initial();
    for (bool & ii : this->queue_flag) {
        ii= false;
    }
}

void Communication::initial(uint16_t* baseAddr,uint16_t len) {
    modbus::Link_UART_CALLback();
    modbus::Link_TIMER_CALLback(this);
    this->Holding = baseAddr;
    this->Holding_num = len;
    modbus::config(baseAddr,len);
    this->set_NVIC(true);
}

uint16_t Communication::find_address(const float *data) {
    return (uint16_t)(data - (float *)this->Holding);
}

uint16_t Communication::find_address(const uint16_t *data) {
    return (uint16_t)(data - this->Holding)*2;
}

void Communication::data_set(uint8_t id,uint16_t address,uint16_t data) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii][0]  = data;
            this->addx[ii][0] = address;
            this->addx[ii][1] = 1;
            this->addx[ii][2] = id;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

void Communication::data_set(uint8_t id,uint16_t address,const uint16_t* data,uint8_t num){
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            // this->datax[ii][0]  = *data;
            // this->datax[ii][1]  = *(data+1);
            for (uint8_t jj=0;jj<num;jj++)
            {
                this->datax[ii][jj]  = data[jj];
            }
            this->addx[ii][0] = address;
            this->addx[ii][1] = num;
            this->addx[ii][2] = id;
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
        this->set_id(this->addx[ii][2]);
        this->modbus_16_send(this->addx[ii][0], this->datax[ii], this->addx[ii][1]);
        this->queue_flag[ii] = false;
    }
    else {
        this->sensordata_sync();
    }
}

void Communication::sensordata_sync() {
    static uint16_t times=0;
    times++;
    if(times>100)
    {
        queue_len=5;
        times=0;
    }
    switch (queue_num)
    {
    case 0:
        this->set_id(1);
        if (this->modbus_03_send(0x101, 6) == modbus::modbus_success) {
            uint16_t *data =this->Holding;
            //this->env.water_pump_state = *data;
            //this->env.fertilizer_pump_state = *(data+1);
        }
    break;
    case 2:
        this->set_id(2);
        if (this->modbus_03_send(0x0, 10) == modbus::modbus_success) {
            uint16_t *data =this->Holding;
            union {
                uint32_t u32;
                float f;
            } flow{};
            flow.u32 = *data + (*(data + 1)<<16);
        }
        break;
    default:
        queue_num=0;
    break;
    }
    queue_num++;
    if(queue_num>=queue_len){queue_num=0;}
}

string Communication::data_to_json(const string& db,const string& str) const {
    string buf;
    //½á¹ûÊ¾Àý
    return buf;
}



