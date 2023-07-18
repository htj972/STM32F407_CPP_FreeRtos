/**
* @Author kokirika
* @Name EC20
* @Date 2023-07-18
*/
#include "EC20.h"
#include "delay.h"

EC20::EC20(USART_TypeDef *USARTx, int32_t bound) {
    this->init(USARTx,bound);
}

EC20::~EC20() {
//    if (this->new_flag)
//        delete this->USART;
}

void EC20::init(USART_TypeDef *USARTx, int32_t bound) {
    this->new_flag = true;
    this->USART=new _USART_(USARTx,bound);
}

void EC20::init(_USART_* USARTx) {
    this->new_flag = false;
    this->USART=USARTx;
}

void EC20::set_delay_times(uint8_t delay_times) {
    this->delay_time=delay_times;
}

bool EC20::Compare(const string& target,const string& data) {
    if(target.find(data)>=0)
        return true;
    else
        return false;
}

string EC20::init() {
    if(this->USART!= nullptr){
        this->USART->write("AT\r\n");
        delay_ms(1000);
        delay_ms(1000);
        delay_ms(1000);
        return this->USART->read_data();
    }
    return "nullptr";
}


