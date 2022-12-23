/**
* @Author kokirika
* @Name Project
* @Date 2022-12-23
**/

#ifndef STM32F407_CPP_FREERTOS_SMI_H
#define STM32F407_CPP_FREERTOS_SMI_H


#include "Temp_Pres_BASE.h"
#include "IIC.h"

class SMI: public Temp_Pres_BASE{
protected:
    Software_IIC *iicx{};
private:
    uint8_t Addr{};
public:
    explicit SMI(Software_IIC *IICx, uint8_t Address=0x50);
    SMI()=default;
    float get_sensor_temp() override;
    float get_sensor_pres() override;
    bool get_sensor_temp_pres(float* Temp,float *Pres) override;
};


#endif //STM32F407_CPP_FREERTOS_SMI_H
