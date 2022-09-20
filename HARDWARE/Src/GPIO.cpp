/**
* @Author kokirika
* @Name GPIO
* @Date 2022-09-13
**/

#include "GPIO.h"

static uint8_t PORTx_RCC[11]; //PORT RCC使能标志

void _GPIO_::RCC_ENABLE() const {
    uint8_t GPIO_NUM=this->get_PORTx_num();
    if(PORTx_RCC[GPIO_NUM]!=ON)
    {
        PORTx_RCC[GPIO_NUM]=ON;
        uint32_t RCC_GPIO_PORTx=(RCC_AHB1Periph_GPIOA << GPIO_NUM);
        RCC_AHB1PeriphClockCmd(RCC_GPIO_PORTx, ENABLE);//使能GPIOx时钟
    }
}

uint8_t _GPIO_::get_PORTx_num() const
{
    return (((uint32_t)this->PORTx-(uint32_t)GPIOA)/0x400);
}

uint8_t _GPIO_::get_pinx_num() const {
    u8 ii=0;
    for(;ii<16;ii++)
    {
        if((this->GPIOx>>ii)==GPIO_Pin_0)
            break;
    }
    return ii;
}

void _GPIO_::init(GPIO_TypeDef* PORT,uint32_t Pin,GPIOMode_TypeDef Mode) {
    this->PORTx=PORT;
    this->GPIOx=Pin;
    this->init_flag=1;
    this->RCC_ENABLE();
    this->GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    this->GPIO_InitStructure.GPIO_Pin = this->GPIOx;
    this->GPIO_InitStructure.GPIO_Mode= Mode;
    this->Pinx=this->get_PORTx_num()*0x10+this->get_pinx_num();
    GPIO_Init(this->PORTx, &this->GPIO_InitStructure);
}

void _GPIO_::init(uint8_t Pin, GPIOMode_TypeDef Mode) {
    uint32_t port_t=reinterpret_cast <uint32_t>(GPIOA)+(Pin/0x10)*0x400;
    this->PORTx=reinterpret_cast <GPIO_TypeDef *>(port_t);
    this->GPIOx=GPIO_Pin_0<<(Pin%0x10);
    this->init(this->PORTx,this->GPIOx,Mode);
}

void _GPIO_::config(GPIOSpeed_TypeDef Speed, GPIOOType_TypeDef OType, GPIOPuPd_TypeDef PuPd) {
    this->GPIO_InitStructure.GPIO_OType= OType;
    this->GPIO_InitStructure.GPIO_PuPd	= PuPd;
    this->GPIO_InitStructure.GPIO_Speed = Speed;
    GPIO_Init(this->PORTx, &this->GPIO_InitStructure);
}

void _GPIO_::set_OTYPE(GPIOOType_TypeDef OType) {
    this->GPIO_InitStructure.GPIO_OType=OType;
    if(this->init_flag==1)
        GPIO_Init(this->PORTx, &this->GPIO_InitStructure);
}

void _GPIO_::set_speed(GPIOSpeed_TypeDef Speed) {
    this->GPIO_InitStructure.GPIO_Speed=Speed;
    if(this->init_flag==1)
        GPIO_Init(this->PORTx, &this->GPIO_InitStructure);
}

void _GPIO_::set_PuPD(GPIOPuPd_TypeDef PuPd) {
    this->GPIO_InitStructure.GPIO_PuPd=PuPd;
    if(this->init_flag==1)
        GPIO_Init(this->PORTx, &this->GPIO_InitStructure);
}
void _GPIO_::set_pinmode(GPIOMode_TypeDef Mode){
    this->GPIO_InitStructure.GPIO_Mode=Mode;
    if(this->init_flag==1)
        GPIO_Init(this->PORTx, &this->GPIO_InitStructure);
}

void _GPIO_::set_output(uint8_t Hi_Lo) const {
    if(Hi_Lo==LOW)
        GPIO_ResetBits(this->PORTx,this->GPIOx);
    else
        GPIO_SetBits(this->PORTx,this->GPIOx);
}

uint8_t _GPIO_::get_output() const {
    return GPIO_ReadOutputDataBit(this->PORTx, this->GPIOx);
}

uint8_t _GPIO_::get_input() const {
    return GPIO_ReadInputDataBit(this->PORTx,this->GPIOx);
}

void _GPIO_::set_AFConfig(uint8_t AF_MODE) const {
    uint16_t Pin_Source=this->get_pinx_num();
    GPIO_PinAFConfig(this->PORTx,Pin_Source,AF_MODE);
}


