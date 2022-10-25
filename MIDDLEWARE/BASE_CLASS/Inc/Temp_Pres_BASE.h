/**
* @Author kokirika
* @Name TEMP_PRES_BASE
* @Date 2022-10-25
**/

#ifndef KOKIRIKA_TEMP_PRES_BASE_H
#define KOKIRIKA_TEMP_PRES_BASE_H

#include "Pressure_BASE.h"
#include "Temperature_BASE.h"

class Temp_Pres_BASE: public Temperature_BASE,public Pressure_BASE{
private:

public:
    explicit Temp_Pres_BASE(float Toffset=0,float Poffset=0);
    ~Temp_Pres_BASE()=default;
    float get_sensor_temp() override;
    float get_sensor_pres() override;
    virtual bool get_sensor_temp_pres(float* Temp,float *Pres);
    bool get_temp_pres(float* Temp,float *Pres);
    void set_offset(float Toffset,float Poffset);
    void calculate_offset(float Temp,float Pres);

};


#endif //KOKIRIKA_TEMP_PRES_BASE_H
