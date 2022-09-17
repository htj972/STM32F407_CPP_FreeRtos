/**
* @Author kokirika
* @Name IRQHandler
* @Date 2022-09-15
**/

#ifndef IRQHANDLER_H
#define IRQHANDLER_H

#include "sys.h"

typedef struct {
    void (*Usart_IRQ_link)(uint8_t channel,uint8_t data);
    void (*extern_IRQ_link)(uint8_t channel,uint8_t data);
}_IRQ_STRUCT_;
extern _IRQ_STRUCT_ HARD_IQR;

typedef struct {
    void (*EXIT_IRQ_link)(uint8_t channel);
    void (*extern_IRQ_link)(uint8_t channel);
}_EXIT_STRUCT_;
extern _EXIT_STRUCT_ EXIT_IQR;
#endif //IRQHANDLER_H
