/**
* @Author kokirika
* @Name TIM_CAPTURE
* @Date 2022-12-09
**/

#include "Tim_Capture.h"
#include "malloc.h"
/*!
 * 构造 无需init config
 * @param TIMx 定时器指针
 * @param FRQ  采样频率 默认 _1M
 */
Tim_Capture::Tim_Capture(TIM_TypeDef *TIMx, uint8_t channelx,GPIO_Pin Pinx, uint32_t fifo_size, uint32_t FRQ) {
    this->init(TIMx,FRQ);
    this->config_Pin(channelx,Pinx,fifo_size);
}
/*!
 * 构造 无需init config
 * @param TIMx 定时器指针
 * @param FRQ  采样频率 默认 _1M
 */
Tim_Capture::Tim_Capture(TIM_TypeDef *TIMx, uint8_t channelx, uint32_t fifo_size, uint32_t FRQ) {
    this->init(TIMx,FRQ);
    this->config_Pin(channelx,fifo_size);
}
/*!
 * 构造 需要 config
 * @param TIMx 定时器指针
 * @param FRQ  采样频率 默认 _1M
 */
Tim_Capture::Tim_Capture(TIM_TypeDef* TIMx,uint32_t FRQ){
    this->init(TIMx,FRQ);
}
/*!
 * 析构函数 释放内存
 */
Tim_Capture::~Tim_Capture() {
    myfree(SRAMIN,*this->CAPTURE_TABLE);
}
/*!
 * 初始化
 * @param TIMx 定时器指针
 * @param FRQ  采样频率 默认 _1M
 */
void Tim_Capture::init(TIM_TypeDef *TIMx, uint32_t FRQ) {
    uint16_t APBx=84;
    if((TIMx==TIM1)||(TIMx==TIM8)||(TIMx==TIM9)||(TIMx==TIM10)||(TIMx==TIM11))
        APBx=168*(1000000/FRQ);
    else if((TIMx==TIM2)||(TIMx==TIM3)||(TIMx==TIM4)||(TIMx==TIM5)||(TIMx==TIM6)
            ||(TIMx==TIM7)||(TIMx==TIM12)||(TIMx==TIM13)||(TIMx==TIM14))
        APBx=84*(1000000/FRQ);

    Timer::init(TIMx,0XFFFFFFFF-1,APBx-1);
}
/*!
 * 配置通道
 * @param channelx 通道
 * @param Pinx  引脚
 * @param fifo_size  申请内存 给缓存区
 */
void Tim_Capture::config_Pin(uint8_t channelx,uint8_t Pinx,uint32_t fifo_size) {
    this->GPIOx.init(Pinx,GPIO_Mode_AF);
    this->GPIOx.set_PuPD(GPIO_PuPd_NOPULL);
    this->GPIO_AF_config();
    TIM_Cmd(this->Timx,DISABLE );
    if(channelx>4)channelx=4;
    else if(channelx==0)channelx=1;
    this->set_Callback();
    this->CAPTURE_MAX_LEN=fifo_size;
    this->CAPTURE_TABLE[fifo_size]=(uint32_t*)mymalloc(SRAMIN,fifo_size*4);
    this->CHANNEL=channelx;
    this->TIM_ICInitStructure.TIM_Channel = 0x0004*channelx; //CC1S=01 	选择输入端 IC映射到TI上
    this->TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
    this->TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI上
    this->TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    this->TIM_ICInitStructure.TIM_ICFilter = 0xf;//IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(this->Timx, &this->TIM_ICInitStructure);

    this->set_NVIC(true);
    TIM_ITConfig(this->Timx,(0x0002<<channelx),DISABLE);//允许更新中断 ,允许CCIE捕获中断
    TIM_SetCounter(this->Timx,0);

}
/*!
 * 配置通道
 * @param channelx 通道
 * @param fifo_size  申请内存 给缓存区
 */
void Tim_Capture::config_Pin(uint8_t channelx,uint32_t fifo_size){
    if(channelx>4)channelx=4;
    else if(channelx==0)channelx=1;
    this->CAPTURE_MAX_LEN=fifo_size;
    this->CAPTURE_TABLE[fifo_size]=(uint32_t*)mymalloc(SRAMIN,fifo_size*4);
    this->CHANNEL=channelx;
    this->default_GPIO(channelx);
    TIM_Cmd(this->Timx,DISABLE );
    this->set_Callback();
    this->TIM_ICInitStructure.TIM_Channel = 0x0004*channelx; //CC1S=01 	选择输入端 IC映射到TI上
    this->TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
    this->TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI上
    this->TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    this->TIM_ICInitStructure.TIM_ICFilter = 0xf;//IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(this->Timx, &this->TIM_ICInitStructure);

    this->set_NVIC(true);
    TIM_ITConfig(this->Timx,(0x0002<<channelx),DISABLE);//允许更新中断 ,允许CCIE捕获中断
    TIM_SetCounter(this->Timx,0);
}
/*!
 * 引脚复用
 */
void Tim_Capture::GPIO_AF_config(){
    if(this->Timx==TIM2)
        this->GPIOx.set_AFConfig(GPIO_AF_TIM2);
    else if(this->Timx==TIM3)
        this->GPIOx.set_AFConfig(GPIO_AF_TIM3);
    else if(this->Timx==TIM4)
        this->GPIOx.set_AFConfig(GPIO_AF_TIM4);
    else if(this->Timx==TIM5)
        this->GPIOx.set_AFConfig(GPIO_AF_TIM5);
    else return;
}
/*!
 * 引脚复用 通道一
 */
bool Tim_Capture::GPIO_CH1() {
    switch (this->timer_num) {
//        case 1:this->GPIOx.init(GPIOE9,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx.init(GPIOA0,GPIO_Mode_AF);break;
        case 3:this->GPIOx.init(GPIOA6,GPIO_Mode_AF);break;//case 13:
        case 4:this->GPIOx.init(GPIOD12,GPIO_Mode_AF);break;
//        case 8:this->GPIOx.init(GPIOC6,GPIO_Mode_AF);break;
//        case 9:this->GPIOx.init(GPIOE5,GPIO_Mode_AF);break;
//        case 10:this->GPIOx.init(GPIOB8,GPIO_Mode_AF);break;
//        case 11:this->GPIOx.init(GPIOB9,GPIO_Mode_AF);break;
//        case 12:this->GPIOx.init(GPIOB14,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}
/*!
 * 引脚复用 通道二
 */
bool Tim_Capture::GPIO_CH2() {
    switch (this->timer_num) {
//        case 1:this->GPIOx.init(GPIOE11,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx.init(GPIOA1,GPIO_Mode_AF);break;
        case 3:this->GPIOx.init(GPIOA7,GPIO_Mode_AF);break;//case 13:
        case 4:this->GPIOx.init(GPIOD13,GPIO_Mode_AF);break;
//        case 8:this->GPIOx.init(GPIOC7,GPIO_Mode_AF);break;
//        case 9:this->GPIOx.init(GPIOE6,GPIO_Mode_AF);break;
//        case 12:this->GPIOx.init(GPIOB15,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}
/*!
 * 引脚复用 通道三
 */
bool Tim_Capture::GPIO_CH3() {
    switch (this->timer_num) {
//        case 1:this->GPIOx.init(GPIOE13,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx.init(GPIOA2,GPIO_Mode_AF);break;
        case 3:this->GPIOx.init(GPIOB0,GPIO_Mode_AF);break;
        case 4:this->GPIOx.init(GPIOD14,GPIO_Mode_AF);break;
//        case 8:this->GPIOx.init(GPIOC8,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}
/*!
 * 引脚复用 通道四
 */
bool Tim_Capture::GPIO_CH4() {
    switch (this->timer_num) {
//        case 1:this->GPIOx.init(GPIOE14,GPIO_Mode_AF);break;
        case 2:case 5:this->GPIOx.init(GPIOA3,GPIO_Mode_AF);break;
        case 3:this->GPIOx.init(GPIOB1,GPIO_Mode_AF);break;
        case 4:this->GPIOx.init(GPIOD15,GPIO_Mode_AF);break;
//        case 8:this->GPIOx.init(GPIOC9,GPIO_Mode_AF);break;
        default:return false;
    }
    return true;
}
/*!
 * 引脚复用 通道一 ~ 四
 */
void Tim_Capture::default_GPIO(uint8_t channelx) {
    switch (channelx) {
        case 1:
            this->GPIO_CH1();
            break;
        case 2:
            this->GPIO_CH2();
            break;
        case 3:
            this->GPIO_CH3();
            break;
        case 4:
            this->GPIO_CH4();
            break;
        default:return;
    }
    this->GPIOx.set_PuPD(GPIO_PuPd_NOPULL);
    this->GPIO_AF_config();
}
/*!
 * CC回调
 */
void Tim_Capture::Callback(int num, char **) {
    uint8_t channelx=num-this->timer_num;
    if(channelx==this->CHANNEL)
    {
        switch(this->CHANNEL){
            case 2:this->CAPTURE_VAL=TIM_GetCapture2(this->Timx);break;
            case 3:this->CAPTURE_VAL=TIM_GetCapture3(this->Timx);break;
            case 4:this->CAPTURE_VAL=TIM_GetCapture4(this->Timx);break;
            default:this->CAPTURE_VAL=TIM_GetCapture1(this->Timx);break;
        }
//        this->CAPTURE_VAL=TIM_GetCapture2(this->Timx);
        TIM_SetCounter(this->Timx,0);
        *this->CAPTURE_TABLE[this->CAPTURE_LEN++]=this->CAPTURE_VAL;
        if(this->CAPTURE_LEN>=this->CAPTURE_MAX_LEN)
            this->CAPTURE_LEN=0;
    }
}
/*!
 * 配置回调
 */
void Tim_Capture::set_Callback() {
    this->upload_extern_fun(this);
    this->set_CCextern_fun(true);
}

uint32_t Tim_Capture::get_CAPTURE_fifo() {
    uint32_t sum=0;
    for(uint32_t ii=0;ii<this->CAPTURE_MAX_LEN;ii++)
        sum+=*this->CAPTURE_TABLE[ii];
    return sum/this->CAPTURE_MAX_LEN;
}

uint32_t Tim_Capture::get_CAPTURE_VAL() const {
    return this->CAPTURE_VAL;
}

void Tim_Capture::start() {
    TIM_Cmd(this->Timx,ENABLE );
}

void Tim_Capture::stop() {
    TIM_Cmd(this->Timx,DISABLE );
}

void Tim_Capture::set_enable(bool en) {
    TIM_Cmd(this->Timx,en?ENABLE:DISABLE );
}




