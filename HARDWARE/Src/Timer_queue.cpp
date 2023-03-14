/**
* @Author kokirika
* @Name TIMER_QUEUE
* @Date 2023-03-13
**/

#include "Timer_queue.h"
/*!
 * ���� ���õ�init
 */
Timer_queue::Timer_queue(TIM_TypeDef *TIMx, uint32_t FRQ) {
    this->init(TIMx,FRQ);
}
/*!
 * ��ʼ����ʱ��
 * @param TIMx ��ʱ��ָ��
 * @param FRQ  Ƶ��
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
 * ���ػص���
 * @param event �ص���ָ��
 */
void Timer_queue::upload_extern_fun(Call_Back *extx) {
    if(this->timer_queue_num<TIMER_QUEUE_MAX){
        this->Ext[this->timer_queue_num++]=extx;
    }
}
/*!
 * ��д�ص��ӿ�
 * @param data ��������ָ��
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
 * ��ȡ��ʱ����
 * @return ��ǰ�����ʱ���� timer_queue_num
 */
uint8_t Timer_queue::get_Timer_num() {
    return this->timer_queue_num-1;
}




