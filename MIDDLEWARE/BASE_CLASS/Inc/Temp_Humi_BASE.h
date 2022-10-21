/**
* @Author kokirika
* @Name Temp_Humi_BASE
* @Date 2022-10-20
**/

#ifndef KOKIRIKA_TEMP_HUMI_BASE_H
#define KOKIRIKA_TEMP_HUMI_BASE_H

#include "Temperature_BASE.h"
#include "Humidity_BASE.h"

class Temp_Humi_BASE: public Temperature_BASE,public Humidity_BASE{
private:

public:
    explicit Temp_Humi_BASE(float Toffset=0,float Hoffset=0);
    ~Temp_Humi_BASE()=default;
    float get_sensor_temp() override;
    float get_sensor_humi() override;
    virtual bool get_sensor_temp_humi(float* Temp,float *Humi);
    bool get_temp_humi(float* Temp,float *Humi);
    void set_offset(float Toffset,float Hoffset);
    void calculate_offset(float Temp,float Humi);
};


#endif //KOKIRIKA_TEMP_HUMI_BASE_H
