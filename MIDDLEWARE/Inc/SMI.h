/**
* @Author kokirika
* @Name SMI
* @Date 2022-12-23
**/

#ifndef KOKIRIKA_SMI_H
#define KOKIRIKA_SMI_H


#include "Temp_Pres_BASE.h"
#include "IIC.h"

class SMI: public Temp_Pres_BASE{
protected:
    Software_IIC *iicx{};
    void  get_temp_pres_value(float* Temp,float *Pres);
private:
    uint8_t Addr{};
    float max{};
    float min{};
public:
    explicit SMI(Software_IIC *IICx,float pmax,float pmin,uint8_t Address=0x50);
    SMI()=default;
    void init(Software_IIC *IICx,float pmax,float pmin,uint8_t Address=0x50);
    float get_sensor_temp() override;
    float get_sensor_pres() override;
    bool get_sensor_temp_pres(float* Temp,float *Pres) override;
};


#endif //KOKIRIKA_SMI_H
