/**
* @Author kokirika
* @Name TEMP_PRES_BASE
* @Date 2022-10-25
**/

#include "Temp_Pres_BASE.h"

Temp_Pres_BASE::Temp_Pres_BASE(float Toffset,float Poffset) {
    this->set_pres_offset(0);
    this->set_temp_offset(0);
}

float Temp_Pres_BASE::get_sensor_temp() {
    return Temperature_BASE::get_sensor_temp();
}

float Temp_Pres_BASE::get_sensor_pres() {
    return Pressure_BASE::get_sensor_pres();
}

bool Temp_Pres_BASE::get_sensor_temp_pres(float *Temp, float *Pres) {
    return false;
}

bool Temp_Pres_BASE::get_temp_pres(float *Temp, float *Pres) {
    float GT,GP;
    bool ret=get_sensor_temp_pres(&GT,&GP);
    *Temp=GT- this->get_temp_offset();
    *Pres=GP- this->get_pres_offset();
    return ret;
}

void Temp_Pres_BASE::set_offset(float Toffset, float Poffset) {
    this->set_temp_offset(Toffset);
    this->set_pres_offset(Poffset);
}

void Temp_Pres_BASE::calculate_offset(float Temp, float Pres) {
    this->calculate_temp_offset(Temp);
    this->calculate_pres_offset(Pres);
}
