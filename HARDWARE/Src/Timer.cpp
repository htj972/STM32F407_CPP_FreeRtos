/**
* @Author kokirika
* @Name TIMER
* @Date 2022-10-31
**/

#include "Timer.h"

#define Timer_Max_num 14


Timer::Timer(TIM_TypeDef *TIMx, uint32_t arr, uint16_t psc, bool nvic) {
    this->init(TIMx, arr, psc, nvic);
}

Timer::Timer() {
    this->config_flag= false;
}

void Timer::init(TIM_TypeDef* TIMx,uint32_t arr,uint16_t psc,bool nvic) {
    this->Timx=TIMx;
    this->timer_num=0;
    this->RCC_init();
    this->config_flag= true;
    Timer::Timer_extern_init();
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    TIM_TimeBaseInit(this->Timx,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
    TIM_Cmd(this->Timx,ENABLE); //ʹ�ܶ�ʱ��3
    if(nvic)
        this->set_NVIC(true);
}

void Timer::RCC_init() {
    if (this->Timx==TIM1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
        this->timer_num=1;
    }
    else if(this->Timx==TIM2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
        this->timer_num=2;
    }
    else if(this->Timx==TIM3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
        this->timer_num=3;
    }
    else if(this->Timx==TIM4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
        this->timer_num=4;
    }
    else if(this->Timx==TIM5) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
        this->timer_num=5;
    }
    else if(this->Timx==TIM6) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
        this->timer_num=6;
    }
    else if(this->Timx==TIM7) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
        this->timer_num=7;
    }
    else if(this->Timx==TIM8) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
        this->timer_num=8;
    }
    else if(this->Timx==TIM9) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
        this->timer_num=9;
    }
    else if(this->Timx==TIM10) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);
        this->timer_num=10;
    }
    else if(this->Timx==TIM11) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);
        this->timer_num=11;
    }
    else if(this->Timx==TIM12) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);
        this->timer_num=12;
    }
    else if(this->Timx==TIM13) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);
        this->timer_num=13;
    }
    else if(this->Timx==TIM14) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);
        this->timer_num=14;
    }
}

void Timer::set_NVIC(bool sata,uint8_t Preemption,uint8_t SubPriority) {

    FunctionalState NVIC_sata=sata?ENABLE:DISABLE;
    TIM_ITConfig(this->Timx,TIM_IT_Update,NVIC_sata); //������ʱ��x�����ж�
    switch (timer_num) {
        case 1:NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_TIM10_IRQn; break;
        case 2:NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; break;
        case 3:NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; break;
        case 4:NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; break;
        case 5:NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; break;
        case 6:NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn; break;
        case 7:NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; break;
        case 8:NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn; break;
        case 9:NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; break;
        case 10:NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_TIM10_IRQn; break;
        case 11:NVIC_InitStructure.NVIC_IRQChannel=TIM1_TRG_COM_TIM11_IRQn; break;
        case 12:NVIC_InitStructure.NVIC_IRQChannel=TIM8_BRK_TIM12_IRQn; break;
        case 13:NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn; break;
        case 14:NVIC_InitStructure.NVIC_IRQChannel=TIM8_TRG_COM_TIM14_IRQn; break;
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=Preemption; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=SubPriority;       //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd=NVIC_sata;
    NVIC_Init(&NVIC_InitStructure);
}
enum extern_MODE{
    C_fun,
    CPP_fun,
};
struct TIMER_STRUCT_{
    bool extern_flag= false;
    void (*funC[Timer_Max_num])(){};
    extern_MODE run_mode[Timer_Max_num]{};
    std::function<void()> funCPP[Timer_Max_num+1];
}TIMER_STRUCT;
void Timer_RUN_VOID() {}

void Timer::Timer_extern_init() {
    if(!TIMER_STRUCT.extern_flag) {
        for(auto & ii : TIMER_STRUCT.funC)
        ii=Timer_RUN_VOID;
        for(auto & ii : TIMER_STRUCT.run_mode)
        ii=extern_MODE::C_fun;
    }
}

void Timer::Timer_extern_upset(uint8_t num)
{
    if(TIMER_STRUCT.run_mode[num%Timer_Max_num]==extern_MODE::C_fun)
        TIMER_STRUCT.funC[num%Timer_Max_num]();
    else
        TIMER_STRUCT.funCPP[num%Timer_Max_num]();
}

void Timer::upload_extern_fun(void (*fun)()) {
    TIMER_STRUCT.funC[this->timer_num%Timer_Max_num]=fun;
    TIMER_STRUCT.run_mode[this->timer_num%Timer_Max_num]=extern_MODE::C_fun;
    this->extern_IRQ_link=fun;
}

void Timer::Timer_extern_fun(std::function<void()> fun) {
    this->localfunxx=fun;
    TIMER_STRUCT.funCPP[this->timer_num%Timer_Max_num]=std::move(fun);
    TIMER_STRUCT.run_mode[this->timer_num%Timer_Max_num]=extern_MODE::CPP_fun;
//    test[this->timer_num>=Timer_Max_num+1?this->timer_num:Timer_Max_num]=std::move(localfunc);
}

bool Timer::get_config() const {
    return this->config_flag;
}



extern "C" {
void TIM1_UP_TIM10_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(1);
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //����жϱ�־λ
    }
    if (TIM_GetITStatus(TIM10, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(10);
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(2);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(3);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(4);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM5_IRQHandler(void) {
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(5);
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM6_DAC_IRQHandler(void) {
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(6);
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM7_IRQHandler(void) {
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(7);
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM8_UP_TIM13_IRQHandler(void) {
    if (TIM_GetITStatus(TIM8, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(8);
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);  //����жϱ�־λ
    }
    if (TIM_GetITStatus(TIM13, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(13);
        TIM_ClearITPendingBit(TIM13, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM1_BRK_TIM9_IRQHandler(void) {
    if (TIM_GetITStatus(TIM9, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(9);
        TIM_ClearITPendingBit(TIM9, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM1_TRG_COM_TIM11_IRQHandler(void) {
    if (TIM_GetITStatus(TIM11, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(11);
        TIM_ClearITPendingBit(TIM11, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM8_BRK_TIM12_IRQHandler(void) {
    if (TIM_GetITStatus(TIM12, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(12);
        TIM_ClearITPendingBit(TIM12, TIM_IT_Update);  //����жϱ�־λ
    }
}
void TIM8_TRG_COM_TIM14_IRQHandler(void) {
    if (TIM_GetITStatus(TIM14, TIM_IT_Update) == SET) //����ж�
    {
        Timer::Timer_extern_upset(14);
        TIM_ClearITPendingBit(TIM14, TIM_IT_Update);  //����жϱ�־λ
    }
}
}