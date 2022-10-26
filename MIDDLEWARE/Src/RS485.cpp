/**
* @Author kokirika
* @Name RS485
* @Date 2022-10-26
**/

#include "RS485.h"
#include <cstdarg>
#include "delay.h"

RS485::RS485(_USART_ *USART,GPIO_TypeDef *PORT,uint32_t Pinx) {
    this->UARTx=USART;
    this->De.init(PORT,Pinx,GPIO_Mode_OUT);
    this->config(1);
}

RS485::RS485(_USART_ *USART, uint8_t Pinx) {
    this->UARTx=USART;
    this->De.init(Pinx,GPIO_Mode_OUT);
    this->config(1);
}

void RS485::init(_USART_ *USART, GPIO_TypeDef *PORT, uint32_t Pinx) {
    this->UARTx=USART;
    this->De.init(PORT,Pinx,GPIO_Mode_OUT);
    this->config(1);
}

void RS485::init(_USART_ *USART, uint8_t Pinx) {
    this->UARTx=USART;
    this->De.init(Pinx,GPIO_Mode_OUT);
    this->config(1);
}

void RS485::init() {
    this->De.set_output(LOW);
    this->config(1);
}

void RS485::config(uint8_t delay_times) {
    this->delay_time=delay_times;
}

void RS485::write(const char *str, uint16_t len) {
    this->De.set_output(HIGH);
    delay_ms(this->delay_time);
    this->UARTx->write(str,len);
    delay_ms(this->delay_time);
    this->De.set_output(HIGH);
}

void RS485::write(uint8_t *str, uint16_t len) {
    this->write((char*)str,len);
}

void RS485::write(const std::string& String) {
    this->write(String.c_str(),String.length());
}

uint16_t RS485::print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    std::string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    this->write(r);
    return r.length();
}

uint16_t RS485::print(const string &String) {
    this->write(String);
    return String.length();
}

uint16_t RS485::print(const char *s) {
    return this->print("%s",s);
}

uint16_t RS485::print(char s) {
    return this->print("%c",s);
}

uint16_t RS485::print(int integer) {
    return this->print("%d",integer);
}




