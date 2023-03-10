/**
* @Author kokirika
* @Name CD4052_H
* @Date 2023-03-08
**/

#ifndef KOKIRIKA_CD4052_H
#define KOKIRIKA_CD4052_H


#include "Out_In_Put.h"
#include "USART.h"

class CD4052:public _USART_ {
private:
    _OutPut_ CS1,CS2;
protected:
    void change(uint8_t channel);
public:
    CD4052(GPIO_Pin CS_Pin1,GPIO_Pin CS_Pin2,USART_TypeDef* USARTx);
    void init(GPIO_Pin CS_Pin1,GPIO_Pin CS_Pin2,USART_TypeDef* USARTx);
    void write_chanel(uint8_t channel,uint8_t *str,uint16_t len);
};


#endif //KOKIRIKA_CD4052_H
