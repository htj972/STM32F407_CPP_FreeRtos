/**
* @Author kokirika
* @Name PWM_BASE
* @Date 2022-11-02
**/

#include "PWM_BASE.h"

void PWM_BASE::set_max_channel(uint8_t num) {
    this->max_channel=num;
}

void PWM_BASE::set_channelx_ccr(uint8_t num, uint32_t ccr) {
}

void PWM_BASE::set_Duty_cycle(uint8_t channel, float Proportion) {
    if(channel>this->max_channel)channel= this->max_channel;
    this->set_channelx_ccr(channel,(uint32_t)(Proportion* (float )this->ARR/100.0f));
}

void PWM_BASE::set_channel1_Duty_cycle(float Proportion) {
    this->set_Duty_cycle(1,Proportion);
}

void PWM_BASE::set_channel2_Duty_cycle(float Proportion) {
    this->set_Duty_cycle(2,Proportion);
}

void PWM_BASE::set_channel3_Duty_cycle(float Proportion) {
    this->set_Duty_cycle(3,Proportion);
}

void PWM_BASE::set_channel4_Duty_cycle(float Proportion) {
    this->set_Duty_cycle(4,Proportion);
}


