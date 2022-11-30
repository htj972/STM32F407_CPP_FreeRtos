/**
* @Author kokirika
* @Name PWM
* @Date 2022-11-01
**/

#include "PWM.h"
#include <cstdarg>

PWM_H::PWM_H(TIM_TypeDef* TIMx,uint32_t FRQ){
    this->init(TIMx,FRQ);
}

void PWM_H::init(TIM_TypeDef *TIMx, uint32_t FRQ) {
    uint16_t APBx=8400;
    if((TIMx==TIM1)||(TIMx==TIM8)||(TIMx==TIM9)||(TIMx==TIM10)||(TIMx==TIM11))
        APBx=16800;
    else if((TIMx==TIM2)||(TIMx==TIM3)||(TIMx==TIM4)||(TIMx==TIM5)||(TIMx==TIM6)
          ||(TIMx==TIM7)||(TIMx==TIM12)||(TIMx==TIM13)||(TIMx==TIM14))
        APBx=8400;
    this->set_max_channel(4);
    this->ARR=10000/FRQ; //  (MFRQ/APB)/frq
    Timer::init(TIMx,this->ARR-1,APBx-1);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性低
    TIM_ARRPreloadConfig(TIMx,ENABLE);//ARPE使能
}

bool PWM_H::config(int ch, ...) {
    uint8_t channel_num[4];
    for(auto & ii : channel_num)
        ii=0;
    uint8_t data_num=0;
    uint8_t error_num=0;
    va_list arg_ptr;
    int nArgValue;
    va_start(arg_ptr, ch);
    channel_num[data_num++]=ch;
    do
    {
        nArgValue = va_arg(arg_ptr, int);
        if(nArgValue<0)nArgValue=0;
        else channel_num[data_num++]=nArgValue;
        if(data_num>3)break;
    } while (nArgValue != 0);
    va_end(arg_ptr);
    for(uint8_t ii=0;ii<data_num;ii++)
        if(this->GPIO_init(channel_num[ii]))error_num++;
    if(error_num>0)return false;
    return true;
}

bool PWM_H::GPIO_init(uint8_t num) {
    if(num>4)num=4;
    if(num==0)num=1;
    if(this->Timx== nullptr)return false;
    bool ret;
    switch (num) {
        case 1:ret=GPIO_CH1();this->set_channel1_ccr(0);break;
        case 2:ret=GPIO_CH2();this->set_channel2_ccr(0);break;
        case 3:ret=GPIO_CH3();this->set_channel3_ccr(0);break;
        case 4:ret=GPIO_CH4();this->set_channel4_ccr(0);break;
        default:return false;
    }
    if(ret)
        GPIO_AF_config(num);
    this->OCinit(num);
    return ret;
}

bool PWM_H::GPIO_CH1() {
    switch (this->timer_num) {
        case 1:this->GPIOx_CH[0].init(GPIOE9,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx_CH[0].init(GPIOA0,GPIO_Mode_AF);break;
        case 3:case 13:this->GPIOx_CH[0].init(GPIOA6,GPIO_Mode_AF);break;
        case 4:this->GPIOx_CH[0].init(GPIOD12,GPIO_Mode_AF);break;
        case 8:this->GPIOx_CH[0].init(GPIOC6,GPIO_Mode_AF);break;
        case 9:this->GPIOx_CH[0].init(GPIOE5,GPIO_Mode_AF);break;
        case 10:this->GPIOx_CH[0].init(GPIOB8,GPIO_Mode_AF);break;
        case 11:this->GPIOx_CH[0].init(GPIOB9,GPIO_Mode_AF);break;
        case 12:this->GPIOx_CH[0].init(GPIOB14,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}

bool PWM_H::GPIO_CH2() {
    switch (this->timer_num) {
        case 1:this->GPIOx_CH[1].init(GPIOE11,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx_CH[1].init(GPIOA1,GPIO_Mode_AF);break;
        case 3:case 13:this->GPIOx_CH[1].init(GPIOA7,GPIO_Mode_AF);break;
        case 4:this->GPIOx_CH[1].init(GPIOD13,GPIO_Mode_AF);break;
        case 8:this->GPIOx_CH[1].init(GPIOC7,GPIO_Mode_AF);break;
        case 9:this->GPIOx_CH[1].init(GPIOE6,GPIO_Mode_AF);break;
        case 12:this->GPIOx_CH[1].init(GPIOB15,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}

bool PWM_H::GPIO_CH3() {
    switch (this->timer_num) {
        case 1:this->GPIOx_CH[2].init(GPIOE13,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx_CH[2].init(GPIOA2,GPIO_Mode_AF);break;
        case 3:this->GPIOx_CH[2].init(GPIOB0,GPIO_Mode_AF);break;
        case 4:this->GPIOx_CH[2].init(GPIOD14,GPIO_Mode_AF);break;
        case 8:this->GPIOx_CH[2].init(GPIOC8,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}

bool PWM_H::GPIO_CH4() {
    switch (this->timer_num) {
        case 1:this->GPIOx_CH[3].init(GPIOE14,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx_CH[3].init(GPIOA3,GPIO_Mode_AF);break;
        case 3:this->GPIOx_CH[3].init(GPIOB1,GPIO_Mode_AF);break;
        case 4:this->GPIOx_CH[3].init(GPIOD15,GPIO_Mode_AF);break;
        case 8:this->GPIOx_CH[3].init(GPIOC9,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}

void PWM_H::GPIO_AF_config(uint8_t channel){
    if(this->Timx==TIM1)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM1);
    else if(this->Timx==TIM2)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM2);
    else if(this->Timx==TIM3)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM3);
    else if(this->Timx==TIM4)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM4);
    else if(this->Timx==TIM5)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM5);
    else if(this->Timx==TIM8)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM8);
    else if(this->Timx==TIM9)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM9);
    else if(this->Timx==TIM10)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM10);
    else if(this->Timx==TIM11)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM11);
    else if(this->Timx==TIM12)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM12);
    else if(this->Timx==TIM13)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM13);
    else if(this->Timx==TIM14)
        this->GPIOx_CH[channel-1].set_AFConfig(GPIO_AF_TIM14);
}

void PWM_H::OCinit(uint8_t num) {
    switch (num) {
        case 1:
            TIM_OC1Init(this->Timx, &TIM_OCInitStructure);
            TIM_OC1PreloadConfig(this->Timx, TIM_OCPreload_Enable);
            break;
        case 2:
            TIM_OC2Init(this->Timx, &TIM_OCInitStructure);
            TIM_OC2PreloadConfig(this->Timx, TIM_OCPreload_Enable);
            break;
        case 3:
            TIM_OC3Init(this->Timx, &TIM_OCInitStructure);
            TIM_OC3PreloadConfig(this->Timx, TIM_OCPreload_Enable);
            break;
        case 4:
            TIM_OC4Init(this->Timx, &TIM_OCInitStructure);
            TIM_OC4PreloadConfig(this->Timx, TIM_OCPreload_Enable);
            break;
        default:return;
    }
}

void PWM_H::config_Pin(PWM_BASE::channel channelx,uint8_t Pinx) {
    this->GPIOx_CH[(channelx-1)%4].init(Pinx,GPIO_Mode_AF);
    GPIO_AF_config(channelx);
    this->OCinit(channelx);
    this->set_channelx_ccr(channelx,0);
}

void PWM_H::config_Pin(PWM_BASE::channel channelx,GPIO_TypeDef *PORTx, uint32_t Pinx) {
    this->GPIOx_CH[(channelx-1)%4].init(PORTx,Pinx,GPIO_Mode_AF);
    GPIO_AF_config(channelx);
    this->OCinit(channelx);
    this->set_channelx_ccr(channelx,0);
}

void PWM_H::set_channelx_ccr(uint8_t num,uint32_t ccr) {
    switch (num) {
        case 1:set_channel1_ccr(ccr);break;
        case 2:set_channel2_ccr(ccr);break;
        case 3:set_channel3_ccr(ccr);break;
        case 4:set_channel4_ccr(ccr);break;
        default:break;
    }
}

void PWM_H::set_channel1_ccr(uint32_t ccr) {
    TIM_SetCompare1(this->Timx,ccr);
}

void PWM_H::set_channel2_ccr(uint32_t ccr) {
    TIM_SetCompare2(this->Timx,ccr);
}

void PWM_H::set_channel3_ccr(uint32_t ccr) {
    TIM_SetCompare3(this->Timx,ccr);
}

void PWM_H::set_channel4_ccr(uint32_t ccr) {
    TIM_SetCompare4(this->Timx,ccr);
}




