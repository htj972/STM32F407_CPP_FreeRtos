/**
* @Author kokirika
* @Name RS485
* @Date 2022-10-26
**/

#ifndef KOKIRIKA_RS485_H
#define KOKIRIKA_RS485_H

#include "USART.h"

class RS485 {
private:
    _GPIO_ De;
    uint8_t delay_time{};
public:
    _USART_ *UARTx{};
    RS485(_USART_ *USART,GPIO_TypeDef *PORT,uint32_t Pinx);
    RS485(_USART_ *USART,uint8_t Pinx);
    RS485()=default;
    ~RS485()=default;
    void init(_USART_ *USART,GPIO_TypeDef *PORT,uint32_t Pinx);
    void init(_USART_ *USART,uint8_t Pinx);
    void init();
    void config(uint8_t delay_times);

    void write(const char *str,uint16_t len);
    void write(uint8_t *str,uint16_t len);
    void write(const string& String);
    uint16_t print(const char *fmt, ...);
    uint16_t print(const string& String);
    uint16_t print(const char *s);
    uint16_t print(char s);
    uint16_t print(int integer);
};


#endif //KOKIRIKA_RS485_H
