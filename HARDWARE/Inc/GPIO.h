/**
* @Author kokirika
* @Name GPIO
* @Date 2022-09-13
**/
#ifndef _GPIO_H_
#define _GPIO_H_

#include "sys.h"

class _GPIO_ {
private:
    GPIO_InitTypeDef  GPIO_InitStructure{
            .GPIO_OType= GPIO_OType_PP,
            .GPIO_PuPd	= GPIO_PuPd_UP,
    };
    uint8_t     init_flag=0;
    void RCC_ENABLE() const;
public:
    GPIO_TypeDef* PORTx;
    uint32_t      GPIOx;
    uint32_t      Pinx;
    uint8_t get_PORTx_num() const;
    uint8_t get_pinx_num() const;

    void init(GPIO_TypeDef* PORT,uint32_t Pin,GPIOMode_TypeDef Mode);
    void init(uint8_t Pin, GPIOMode_TypeDef Mode);
    void config(GPIOSpeed_TypeDef Speed,GPIOOType_TypeDef OType,GPIOPuPd_TypeDef PuPd);
    void set_speed(GPIOSpeed_TypeDef Speed);
    void set_OTYPE(GPIOOType_TypeDef OType);
    void set_PuPD(GPIOPuPd_TypeDef PuPd);
    void set_pinmode(GPIOMode_TypeDef Mode);
    void set_output(uint8_t Hi_Lo) const;
    uint8_t get_output() const;
    uint8_t get_input() const;

    void set_AFConfig(uint8_t AF_MODE) const;
};



#endif //_GPIO_H_
