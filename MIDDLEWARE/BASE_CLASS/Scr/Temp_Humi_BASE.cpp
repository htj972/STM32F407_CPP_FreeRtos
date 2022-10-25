/**
* @Author kokirika
* @Name Temp_Humi_BASE
* @Date 2022-10-20
**/

#include "Temp_Humi_BASE.h"

Temp_Humi_BASE::Temp_Humi_BASE(float Toffset,float Hoffset) {
    this->set_humi_offset(0);
    this->set_temp_offset(0);
}

float Temp_Humi_BASE::get_sensor_temp() {
    return Temperature_BASE::get_sensor_temp();
}

float Temp_Humi_BASE::get_sensor_humi() {
    return Humidity_BASE::get_sensor_humi();
}

bool Temp_Humi_BASE::get_sensor_temp_humi(float *Temp, float *Humi) {
    return false;
}

bool Temp_Humi_BASE::get_temp_humi(float *Temp, float *Humi) {
    float GT,GH;
    bool ret =get_sensor_temp_humi(&GT,&GH);
    *Temp=GT- this->get_temp_offset();
    *Humi=GH- this->get_humi_offset();
    return ret;
}

void Temp_Humi_BASE::set_offset(float Toffset, float Hoffset) {
    this->set_temp_offset(Toffset);
    this->set_humi_offset(Hoffset);
}

void Temp_Humi_BASE::calculate_offset(float Temp, float Humi) {
    this->calculate_temp_offset(Temp);
    this->calculate_humi_offset(Humi);
}


