/**
* @Author kokirika
* @Name USART_BASE
* @Date 2022-10-26
**/

#include "Printer_usart_BASE.h"

void Printer_usart_BASE::init(_USART_ *UARTx) {
    this->UART=UARTx;
}

void Printer_usart_BASE::write(uint8_t *str,uint16_t len) {
    this->UART->write(str,len);
}

void Printer_usart_BASE::write(const char *str, uint16_t len) {
    this->UART->write(str,len);
}

void Printer_usart_BASE::write(const string &String) {
    this->UART->write(String);
}

void Printer_usart_BASE::write(int integer) {
    this->UART->write((uint8_t *)&integer,1);
}

