/**
* @Author kokirika
* @Name USART_BASE
* @Date 2022-10-26
**/

#include "Printer_usart_BASE.h"

void Printer_usart_BASE::init(_USART_ *UARTx) {
    this->UART=UARTx;
}

void Printer_usart_BASE::write(const char *str, uint16_t len) {
    this->UART->write(str,len);
}
