/**
* @Author kokirika
* @Name CD4052_H
* @Date 2023-03-08
**/

#include "CD4052.h"

CD4052::CD4052(GPIO_Pin CS_Pin1, GPIO_Pin CS_Pin2, USART_TypeDef *USARTx) {
    this->init(CS_Pin1,CS_Pin2,USARTx);
}

void CD4052::init(GPIO_Pin CS_Pin1, GPIO_Pin CS_Pin2, USART_TypeDef *USARTx) {
    CS1.init(CS_Pin1,HIGH);
    CS2.init(CS_Pin2,HIGH);
    _USART_::init(USARTx);
    this->change(1);
}

void CD4052::change(uint8_t channel) {
    switch(channel){
        case 2:
            CS1.set(ON);
            CS2.set(OFF);
            break;
        case 3:
            CS1.set(ON);
            CS2.set(ON);
            break;
        case 4:
            CS1.set(OFF);
            CS2.set(ON);
            break;
        default:
            CS1.set(OFF);
            CS2.set(OFF);
            break;
    }
}

void CD4052::write_chanel(uint8_t channel, uint8_t *str, uint16_t len) {
    this->change(channel);
    _USART_::write(str,len);
}



