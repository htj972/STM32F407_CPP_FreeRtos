/**
* @Author kokirika
* @Name USART_BASE
* @Date 2022-10-26
**/

#ifndef KOKIRIKA_PRINTER_USART_BASE_H
#define KOKIRIKA_PRINTER_USART_BASE_H

#include "Printer_BASE.h"
#include "USART.h"

class Printer_usart_BASE: public Printer_BASE{
private:
    _USART_ *UART{};
public:
    void init(_USART_ *UARTx);
    void write(uint8_t *str,uint16_t len) override;
    void write(const char *str,uint16_t len) override;
    void write(const std::string& String) override;
    void write(int integer) override;
};




#endif //KOKIRIKA_PRINTER_USART_BASE_H
