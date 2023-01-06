/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/

#include "Communication.h"

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq, uint8_t PinTx,
                             uint8_t PinRx) {
    RS485::init(USARTx,DE);
    RS485::config(PinTx,PinRx);
    Timer::init(TIMx,10000/frq,8400, true);
    modbus::init(this);
    this->initial();
}

void Communication::initial() {
    modbus::Link_UART_CALLback();
    modbus::Link_TIMER_CALLback(this);
    modbus::config(this->data_BUS.to_u16,sizeof(this->data_BUS.to_u16)/2);
}

