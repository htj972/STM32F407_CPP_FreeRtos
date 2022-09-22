/**
* @Author kokirika
* @Name OUTPUT
* @Date 2022-09-13
**/

#include "Out_In_Put.h"

void _OutPut_::init(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo) {
    this->GPIO.init(PORTx,Pinx,GPIO_Mode_OUT);
    this->default_mode=Hi_Lo;
    this->GPIO.set_output(!this->default_mode);
}

void _OutPut_::init(uint8_t Pinx, uint8_t Hi_Lo) {
    this->GPIO.init(Pinx,GPIO_Mode_OUT);
    this->default_mode=Hi_Lo;
    this->GPIO.set_output(!this->default_mode);
}

void _OutPut_::set(uint8_t ON_OFF) {
    this->value=ON_OFF;
    if(ON_OFF==OFF)
        this->GPIO.set_output(this->default_mode);
    else
        this->GPIO.set_output(!this->default_mode);
}

void _OutPut_::set_value(uint8_t Hi_Lo) {
    this->value=(Hi_Lo==this->default_mode)?ON:OFF;
    this->GPIO.set_output(Hi_Lo);
}

void _OutPut_::change() {
    this->value=!this->value;
    if(this->value==OFF)
        this->GPIO.set_output(this->default_mode);
    else
        this->GPIO.set_output(!this->default_mode);
}

uint8_t _OutPut_::get() const {
    return this->value;
}

/***********************************************************************/
/**
* @Author kokirika
* @Name INPUT
* @Date 2022-09-13
**/
static bool DOWN_STATE[16];
static uint8_t extern_flag=0;
void EXIT_RUN_VOID(uint8_t) {}
struct _EXIT_STRUCT_{
    void (*EXIT_IRQ_link)(uint8_t channel);
    void (*extern_IRQ_link)(uint8_t channel);
}EXIT_IQR;
//extern _EXIT_STRUCT_ EXIT_IQR;

void _InPut_::init(GPIO_TypeDef *PORTx, uint32_t Pinx, uint8_t Hi_Lo) {
    this->Down_level=Hi_Lo;
    if(this->Down_level!=LOW)
        this->GPIO.set_PuPD(GPIO_PuPd_DOWN);
    this->GPIO.init(PORTx,Pinx,GPIO_Mode_IN);
}

void _InPut_::init(uint8_t Pinx, uint8_t Hi_Lo) {
    this->Down_level=Hi_Lo;
    if(this->Down_level!=LOW)
        this->GPIO.set_PuPD(GPIO_PuPd_DOWN);
    this->GPIO.init(Pinx,GPIO_Mode_IN);
}

void _InPut_::set_EXTI() {
    if(this->GPIO.get_PORTx_num()!=0)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
    this->pin_num=this->GPIO.get_pinx_num();
    SYSCFG_EXTILineConfig(this->GPIO.get_PORTx_num(),this->pin_num);

    if(extern_flag==0) {
        _InPut_::upload_extern_fun(EXIT_RUN_VOID);
    }
    EXIT_IQR.EXIT_IRQ_link=_InPut_::send_to_fifo;

    this->EXTI_InitStructure.EXTI_Line = EXTI_Line0<<this->pin_num;//LINEx
    this->EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
    if(this->Down_level==LOW)
        this->EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    else
        this->EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //下降沿触发
    this->EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINEx
    EXTI_Init(&this->EXTI_InitStructure);//配置

    switch(this->pin_num)
    {
        case 0:NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;break;//外部中断0
        case 1:NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;break;//外部中断1
        case 2:NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;break;//外部中断2
        case 3:NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;break;//外部中断3
        case 4:NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;break;//外部中断4
        case 5:case 6:case 7:case 8:case 9:
            NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;break;//外部中断5-9
        case 10:case 11:case 12:case 13:case 14:case 15:
            NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;break;//外部中断10-15
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}

void _InPut_::setNVIC(uint8_t Priority, uint8_t SubPriority, bool EnAble) {
    this->NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=Priority;
    this->NVIC_InitStructure.NVIC_IRQChannelSubPriority=SubPriority;
    NVIC_Init(&this->NVIC_InitStructure);
    this->setNVIC_ENABLE(EnAble);
}

void _InPut_::setNVIC_ENABLE(bool EnAble) {
    this->EXTI_InitStructure.EXTI_LineCmd = EnAble ? ENABLE : DISABLE;//使能LINEx
    EXTI_Init(&this->EXTI_InitStructure);//配置
}

void _InPut_::upload_extern_fun(void (*fun)(uint8_t)) {
    EXIT_IQR.extern_IRQ_link=fun;
    extern_flag=1;
}

void _InPut_::send_to_fifo(uint8_t channel) {
    DOWN_STATE[channel]=true;
}

uint8_t _InPut_::get_level() {
    return this->GPIO.get_input();
}

bool _InPut_::get_state() {
    return (this->GPIO.get_input() == this->Down_level);
}

bool _InPut_::get_NVIC_state() const {
    return DOWN_STATE[this->pin_num];
}



extern "C" void EXTI0_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line0)==SET){
        EXIT_IQR.EXIT_IRQ_link(0);
        EXIT_IQR.extern_IRQ_link(0);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

extern "C" void EXTI1_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line1)==SET){
        EXIT_IQR.EXIT_IRQ_link(1);
        EXIT_IQR.extern_IRQ_link(1);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

extern "C" void EXTI2_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line2)==SET){
        EXIT_IQR.EXIT_IRQ_link(2);
        EXIT_IQR.extern_IRQ_link(2);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

extern "C" void EXTI3_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line3)==SET){
        EXIT_IQR.EXIT_IRQ_link(3);
        EXIT_IQR.extern_IRQ_link(3);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}
extern "C" void EXTI4_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line4)==SET){
        EXIT_IQR.EXIT_IRQ_link(4);
        EXIT_IQR.extern_IRQ_link(4);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

extern "C" void EXTI9_5_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line5)==SET){
        EXIT_IQR.EXIT_IRQ_link(5);
        EXIT_IQR.extern_IRQ_link(5);
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line6)==SET){
        EXIT_IQR.EXIT_IRQ_link(6);
        EXIT_IQR.extern_IRQ_link(6);
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if(EXTI_GetITStatus(EXTI_Line7)==SET){
        EXIT_IQR.EXIT_IRQ_link(7);
        EXIT_IQR.extern_IRQ_link(7);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
    if(EXTI_GetITStatus(EXTI_Line8)==SET){
        EXIT_IQR.EXIT_IRQ_link(8);
        EXIT_IQR.extern_IRQ_link(8);
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line9)==SET){
        EXIT_IQR.EXIT_IRQ_link(9);
        EXIT_IQR.extern_IRQ_link(9);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

extern "C" void EXTI15_10_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line10)==SET){
        EXIT_IQR.EXIT_IRQ_link(10);
        EXIT_IQR.extern_IRQ_link(10);
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line11)==SET){
        EXIT_IQR.EXIT_IRQ_link(11);
        EXIT_IQR.extern_IRQ_link(11);
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if(EXTI_GetITStatus(EXTI_Line12)==SET){
        EXIT_IQR.EXIT_IRQ_link(12);
        EXIT_IQR.extern_IRQ_link(12);
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if(EXTI_GetITStatus(EXTI_Line13)==SET){
        EXIT_IQR.EXIT_IRQ_link(13);
        EXIT_IQR.extern_IRQ_link(13);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if(EXTI_GetITStatus(EXTI_Line14)==SET){
        EXIT_IQR.EXIT_IRQ_link(14);
        EXIT_IQR.extern_IRQ_link(14);
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15)==SET){
        EXIT_IQR.EXIT_IRQ_link(15);
        EXIT_IQR.extern_IRQ_link(15);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}