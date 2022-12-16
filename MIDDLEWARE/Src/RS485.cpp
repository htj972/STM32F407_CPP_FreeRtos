/**
* @Author kokirika
* @Name RS485
* @Date 2022-10-26
**/

#include "RS485.h"
#include <cstdarg>
#include "delay.h"

RS485::RS485(USART_TypeDef* USARTx,GPIO_TypeDef *PORT,uint32_t Pinx,int32_t bound) {
    this->init(USARTx,PORT,Pinx,bound);
}

RS485::RS485(USART_TypeDef* USARTx, uint8_t Pinx,int32_t bound) {
    this->init(USARTx,Pinx,bound);
}

void RS485::init(USART_TypeDef* USARTx,GPIO_TypeDef *PORT,uint32_t Pinx,int32_t bound) {
    _USART_::init(USARTx,bound);
    this->De.init(PORT,Pinx,HIGH);
    this->set_delay_times(1);
}

void RS485::init(USART_TypeDef* USARTx,uint8_t Pinx,int32_t bound) {
    _USART_::init(USARTx,bound);
    this->De.init(Pinx,HIGH);
    this->set_delay_times(1);
}

void RS485::init() {
    if(this->USART!= nullptr){
        this->De.set_value(LOW);
        this->set_delay_times(this->delay_time);
    }
}

void RS485::set_delay_times(uint8_t delay_times) {
    this->delay_time=delay_times;
}


void RS485::write(const char *str, uint16_t len) {
    this->De.set_value(HIGH);
    delay_ms(this->delay_time);
    _USART_::write(str,len);
    delay_ms(this->delay_time);
    this->De.set_value(LOW);
}

void RS485::write(uint8_t *str, uint16_t len) {
    this->write((char*)str,len);
}

void RS485::write(string String) {
    this->write(String.c_str(),String.length());
}



