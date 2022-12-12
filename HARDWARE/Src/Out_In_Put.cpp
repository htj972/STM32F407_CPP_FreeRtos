/**
* @Author kokirika
* @Name OUTPUT
* @Date 2022-09-13
**/

#include <functional>
#include "Out_In_Put.h"

_OutPut_::_OutPut_(GPIO_TypeDef *PORTx, uint32_t Pinx, uint8_t Hi_Lo) {
    this->init(PORTx,Pinx,Hi_Lo);
}

_OutPut_::_OutPut_(uint8_t Pinx,uint8_t Hi_Lo) {
    this->init(Pinx,Hi_Lo);
}

_OutPut_::_OutPut_(){
    this->setted = false;
}

void _OutPut_::init(){
    if(this->setted)
    {
        if(this->GPIO.Pinx==0xff)
            this->GPIO.get_pinx_num();
        this->GPIO.init(this->GPIO.Pinx,GPIO_Mode_OUT);
        this->GPIO.set_output(!this->default_mode);
    }
}

void _OutPut_::init(GPIO_TypeDef* PORTx,uint32_t Pinx,uint8_t Hi_Lo) {
    this->GPIO.init(PORTx,Pinx,GPIO_Mode_OUT);
    this->default_mode=Hi_Lo;
    this->GPIO.set_output(!this->default_mode);
    this->setted = true;
}

void _OutPut_::init(uint8_t Pinx, uint8_t Hi_Lo) {
    this->GPIO.init(Pinx,GPIO_Mode_OUT);
    this->default_mode=Hi_Lo;
    this->GPIO.set_output(!this->default_mode);
    this->setted = true;
}

void _OutPut_::set(uint8_t ON_OFF) {
    this->value=ON_OFF;
    if(ON_OFF==ON)
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
    this->GPIO.set_change();
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

_InPut_::_InPut_(GPIO_TypeDef *PORTx, uint32_t Pinx, uint8_t Hi_Lo) {
    this->init(PORTx,Pinx,Hi_Lo);
}

_InPut_::_InPut_(uint8_t Pinx, uint8_t Hi_Lo) {
    this->init(Pinx,Hi_Lo);
}

_InPut_::_InPut_() {
    this->setted= false;
}

void _InPut_::init(){
    if(this->setted)
    {
        if(this->GPIO.Pinx==0xff)
            this->GPIO.get_pinx_num();
        this->GPIO.init(this->GPIO.Pinx,GPIO_Mode_IN);
        this->pin_num=this->GPIO.get_GPIOx_num();
    }
}

void _InPut_::init(GPIO_TypeDef *PORTx, uint32_t Pinx, uint8_t Hi_Lo) {
    this->Down_level=Hi_Lo;
    this->GPIO.init(PORTx,Pinx,GPIO_Mode_IN);
    this->pin_num=this->GPIO.get_GPIOx_num();
    if(this->Down_level!=LOW)
        this->GPIO.set_PuPD(GPIO_PuPd_DOWN);
    this->setted= true;
}

void _InPut_::init(uint8_t Pinx, uint8_t Hi_Lo) {
    this->Down_level=Hi_Lo;
    this->GPIO.init(Pinx,GPIO_Mode_IN);
    this->pin_num=this->GPIO.get_GPIOx_num();
    if(this->Down_level!=LOW)
        this->GPIO.set_PuPD(GPIO_PuPd_DOWN);
    this->setted= true;
}

void _InPut_::set_EXTI() {
    if(this->GPIO.get_PORTx_num()!=0)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
    SYSCFG_EXTILineConfig(this->GPIO.get_PORTx_num(),this->pin_num);
    _InPut_::extern_init();

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

#define INPUT_Max_num 16
struct INPUT_STRUCT_{
    bool extern_flag= false;
    Call_Back::MODE run_mode[INPUT_Max_num]{};
    void (*funC[INPUT_Max_num])(){};
    void (*funC_r[INPUT_Max_num])(uint8_t){};
    std::function<void()> funCPP[INPUT_Max_num+1];
    Call_Back *ext[INPUT_Max_num]{};
    bool NVIC_state[INPUT_Max_num]{};
}INPUT_STRUCT;
void INPUT_RUN_VOID() {}

char _InPut_::Interrupt_name[2];
char _InPut_::Interrupt_channel[2];
char* _InPut_::Interrupt_ret[2];

void _InPut_::extern_init() {
    if(!INPUT_STRUCT.extern_flag) {
        INPUT_STRUCT.extern_flag = true;
        for(auto & ii : INPUT_STRUCT.funC)
            ii=INPUT_RUN_VOID;
        for(auto & ii : INPUT_STRUCT.run_mode)
            ii=Call_Back::MODE::C_fun;
        _InPut_::Interrupt_ret[0]=_InPut_::Interrupt_name;
        _InPut_::Interrupt_ret[1]=_InPut_::Interrupt_channel;
        _InPut_::Interrupt_name[0]=Call_Back::Name::exit;
    }
}

void _InPut_::extern_upset(uint8_t num)
{
    INPUT_STRUCT.NVIC_state[num%INPUT_Max_num] = true;
    if(INPUT_STRUCT.run_mode[num%INPUT_Max_num]==Call_Back::MODE::C_fun)
        INPUT_STRUCT.funC[num%INPUT_Max_num]();
    else if(INPUT_STRUCT.run_mode[num%INPUT_Max_num]==Call_Back::MODE::C_fun_r)
        INPUT_STRUCT.funC_r[num%INPUT_Max_num](num);
    else if(INPUT_STRUCT.run_mode[num%INPUT_Max_num]==Call_Back::MODE::CPP_fun)
        INPUT_STRUCT.funCPP[num%INPUT_Max_num]();
    else if(INPUT_STRUCT.run_mode[num%INPUT_Max_num]==Call_Back::MODE::class_fun)
    {
        _InPut_::Interrupt_channel[0]=num;
        INPUT_STRUCT.ext[num%INPUT_Max_num]->Callback(2, _InPut_::Interrupt_ret);
    }
}

void _InPut_::upload_extern_fun(void (*fun)()) {
    INPUT_STRUCT.funC[this->pin_num%INPUT_Max_num]=fun;
    INPUT_STRUCT.run_mode[this->pin_num%INPUT_Max_num]=Call_Back::MODE::C_fun;
    this->extern_IRQ_link=fun;
}

void _InPut_::upload_extern_fun(void(* fun)(uint8_t)){
    INPUT_STRUCT.funC_r[this->pin_num%INPUT_Max_num]=fun;
    INPUT_STRUCT.run_mode[this->pin_num%INPUT_Max_num]=Call_Back::MODE::C_fun_r;
    this->extern_IRQ_link_r=fun;
}

void _InPut_::upload_extern_fun(std::function<void()> fun) {
    this->localfunxx=fun;
    INPUT_STRUCT.funCPP[this->pin_num%INPUT_Max_num]=std::move(fun);
    INPUT_STRUCT.run_mode[this->pin_num%INPUT_Max_num]=Call_Back::MODE::CPP_fun;
}

void _InPut_::upload_extern_fun(Call_Back *extx) const {
    INPUT_STRUCT.ext[this->pin_num%INPUT_Max_num]=extx;
    INPUT_STRUCT.run_mode[this->pin_num%INPUT_Max_num]=Call_Back::MODE::class_fun;
}

uint8_t _InPut_::get_level() {
    return this->GPIO.get_input();
}

bool _InPut_::get_state() {
    return (this->GPIO.get_input() == this->Down_level);
}

bool _InPut_::get_NVIC_state() const {
    bool get_d = INPUT_STRUCT.NVIC_state[this->pin_num];
    INPUT_STRUCT.NVIC_state[this->pin_num] = false;
    return get_d;
}

uint8_t _InPut_::get_pin_num() const {
    return this->pin_num;
}


extern "C" void EXTI0_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line0)==SET){
        _InPut_::extern_upset(0);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

extern "C" void EXTI1_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line1)==SET){
        _InPut_::extern_upset(1);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

extern "C" void EXTI2_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line2)==SET){
        _InPut_::extern_upset(2);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

extern "C" void EXTI3_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line3)==SET){
        _InPut_::extern_upset(3);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}
extern "C" void EXTI4_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line4)==SET){
        _InPut_::extern_upset(4);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

extern "C" void EXTI9_5_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line5)==SET){
        _InPut_::extern_upset(5);
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line6)==SET){
        _InPut_::extern_upset(6);
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if(EXTI_GetITStatus(EXTI_Line7)==SET){
        _InPut_::extern_upset(7);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
    if(EXTI_GetITStatus(EXTI_Line8)==SET){
        _InPut_::extern_upset(8);
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line9)==SET){
        _InPut_::extern_upset(9);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

extern "C" void EXTI15_10_IRQHandler(){                    //外部中断0
    if(EXTI_GetITStatus(EXTI_Line10)==SET){
        _InPut_::extern_upset(10);
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line11)==SET){
        _InPut_::extern_upset(11);
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if(EXTI_GetITStatus(EXTI_Line12)==SET){
        _InPut_::extern_upset(12);
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if(EXTI_GetITStatus(EXTI_Line13)==SET){
        _InPut_::extern_upset(13);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if(EXTI_GetITStatus(EXTI_Line14)==SET){
        _InPut_::extern_upset(14);
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15)==SET){
        _InPut_::extern_upset(15);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}