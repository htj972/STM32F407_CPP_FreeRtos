/**
* @Author kokirika
* @Name RS485
* @Date 2022-10-26
**/

#ifndef KOKIRIKA_RS485_H
#define KOKIRIKA_RS485_H

#include "USART.h"
#include "Out_In_Put.h"

class RS485: public _USART_{
private:
    _OutPut_ De;
    uint8_t delay_time{};
public:
    RS485(USART_TypeDef* USARTx,GPIO_TypeDef *PORT,uint32_t Pinx,int32_t bound=9600);
    RS485(USART_TypeDef* USARTx,uint8_t Pinx,int32_t bound=9600);
    RS485()=default;
    ~RS485()=default;
    void init(USART_TypeDef* USARTx,GPIO_TypeDef *PORT,uint32_t Pinx,int32_t bound=9600);
    void init(USART_TypeDef* USARTx,uint8_t Pinx,int32_t bound=9600);
    void init();
    void set_delay_times(uint8_t delay_times);

    void write(const char *str,uint16_t len) override;
    void write(uint8_t *str,uint16_t len) override;
    void write(string String) override;
};


#endif //KOKIRIKA_RS485_H
