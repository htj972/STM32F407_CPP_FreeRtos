/**
* @Author kokirika
* @Name TIMER_QUEUE
* @Date 2023-03-13
**/

#include "Timer_queue.h"
/*!
 * 构造 调用的init
 */
Timer_queue::Timer_queue(TIM_TypeDef *TIMx, uint32_t FRQ) {
    this->init(TIMx,FRQ);
}
/*!
 * 初始化定时器
 * @param TIMx 定时器指针
 * @param FRQ  频率
 */
void Timer_queue::init(TIM_TypeDef *TIMx, uint32_t FRQ) {
    uint16_t APBx=84;
    if((TIMx==TIM1)||(TIMx==TIM8)||(TIMx==TIM9)||(TIMx==TIM10)||(TIMx==TIM11))
        APBx=168;
    else if((TIMx==TIM2)||(TIMx==TIM3)||(TIMx==TIM4)||(TIMx==TIM5)||(TIMx==TIM6)
            ||(TIMx==TIM7)||(TIMx==TIM12)||(TIMx==TIM13)||(TIMx==TIM14))
        APBx=84;
    uint32_t ARR=1000000/FRQ; //  (MFRQ/APB)/frq
    Timer::init(TIMx,ARR-1,APBx-1, true);
    this->timer_queue_num=0;
    Timer::upload_extern_fun(this);
}
/*!
 * 加载回调类
 * @param event 回调类指针
 */
void Timer_queue::upload_extern_fun(Call_Back *extx) {
    if(this->timer_queue_num<TIMER_QUEUE_MAX){
        this->Ext[this->timer_queue_num++]=extx;
    }
}
/*!
 * 复写回调接口
 * @param data 返回类型指针
 */
void Timer_queue::Callback(int, char **data) {
    if(data[0][0]==Call_Back::Name::timer)
        if(data[1][0]==this->timer_num)
        {
            this->set_Cmd(false);
            for(uint8_t ii=0;ii<timer_queue_num;ii++)
                if(this->Ext[ii]!= nullptr)
                {
                    Timer::Interrupt_name[0]=Call_Back::Name::timer;
                    Timer::Interrupt_channel[0]=ii;
                    Timer::Interrupt_data[0] = 0;
                    this->Ext[ii]->Callback(2, (char**)Timer::Interrupt_ret);
                }
            this->set_Cmd(true);
        }
}
/*!
 * 获取定时器号
 * @return 当前软件定时器号 timer_queue_num
 */
uint8_t Timer_queue::get_Timer_num() {
    return this->timer_queue_num-1;
}




