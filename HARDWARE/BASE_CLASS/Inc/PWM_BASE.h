/**
* @Author kokirika
* @Name PWM_BASE
* @Date 2022-11-02
**/

#ifndef KOKIRIKA_PWM_BASE_H
#define KOKIRIKA_PWM_BASE_H

#include "sys.h"

class PWM_BASE {
private:

protected:
    uint32_t ARR{};
    uint8_t max_channel{};
    void set_max_channel(uint8_t num);
    uint8_t get_max_channel() const;
public:
    enum channel :uint8_t{
        _1 = 1,
        _2 = 2,
        _3 = 3,
        _4 = 4,
    };

    virtual void set_channelx_ccr(uint8_t num,uint32_t ccr);
    void set_Duty_cycle(uint8_t channel,float Proportion);
    void set_channel1_Duty_cycle(float Proportion);
    void set_channel2_Duty_cycle(float Proportion);
    void set_channel3_Duty_cycle(float Proportion);
    void set_channel4_Duty_cycle(float Proportion);
};


#endif //KOKIRIKA_PWM_BASE_H
