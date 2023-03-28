/**
* @Author kokirika
* @Name PRETREATMENT
* @Date 2022-12-17
**/

#ifndef KOKIRIKA_PRETREATMENT_H
#define KOKIRIKA_PRETREATMENT_H


#include "Temp_ctrl.h"
#include "MAX31865.h"
#include "PWM.h"

class pretreatment: public Temp_ctrl, private MAX31865{
private:
    uint8_t SCpinx{};
    bool en;
    uint16_t _05s_back_times;
protected:
    PWM_H *PWMx{};
//    _OutPut_ BENGx;
//    _OutPut_ FAx;
public:
    _OutPut_ BENGx;
    _OutPut_ FAx;
    pretreatment(SPI_BASE *SPIx,uint8_t CSpin,PWM_H *PWM,uint8_t ch,GPIO_Pin BENG,GPIO_Pin FA);
    void initial();

    void turn_ON();
    void turn_OFF();
    void upset();


};


#endif //KOKIRIKA_PRETREATMENT_H
