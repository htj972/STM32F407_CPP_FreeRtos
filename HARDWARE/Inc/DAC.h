/**
* @Author kokirika
* @Name DAC
* @Date 2022-09-22
**/

#ifndef Kokirika_DAC_H
#define Kokirika_DAC_H

#include "GPIO.h"

class _DAC_ {
private:
    _GPIO_  GPIO;
    uint8_t channel;
    DAC_InitTypeDef DAC_InitType;
    void default_config();

public:
    void init(uint8_t channelx);
    void set_value(uint16_t value) const;
    void set_vol_mv(uint16_t mv) const;
};



#endif //Kokirika_DAC_H
