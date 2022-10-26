/**
* @Author kokirika
* @Name ONE_WIRE
* @Date 2022-10-25
**/

#ifndef KOKIRIKA_ONE_WIRE_H
#define KOKIRIKA_ONE_WIRE_H

#include "GPIO.h"

class ONE_Wire {
private:
    _GPIO_ IO;
    enum MODE{
        OUT,
        IN,
    };
    uint16_t err_times{};
    uint8_t IO_mode_flag{};
protected:
    bool config_f{};
    bool Check();
    uint16_t get_err() const;
    bool     check_err(uint16_t err) const;
    uint8_t  get_in();
public:
    void config(uint8_t Pinx,uint16_t err_t=250);
    void config(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,uint16_t err_t=250);
    void SET_IN();
    void SET_OUT(uint8_t Hi_Lo);
};


#endif //KOKIRIKA_ONE_WIRE_H
