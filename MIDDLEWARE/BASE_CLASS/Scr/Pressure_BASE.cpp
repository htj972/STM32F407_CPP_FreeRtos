/**
* @Author kokirika
* @Name PRESSURE_BASE
* @Date 2022-10-25
**/

#include "Pressure_BASE.h"

Pressure_BASE::Pressure_BASE(float offset) {
    this->pres_offset=offset;
}

float Pressure_BASE::get_pres() {
    this->pres_cache=this->get_sensor_pres()-this->pres_offset;
    return this->pres_cache;
}

float Pressure_BASE::get_sensor_pres() {
    return 0;
}

void Pressure_BASE::set_pres_offset(float offset) {
    this->pres_offset=offset;
}

float Pressure_BASE::get_pres_offset() const {
    return this->pres_offset;
}

void Pressure_BASE::calculate_pres_offset(float pres) {
    this->pres_offset=this->get_sensor_pres()-pres;
}

float Pressure_BASE::get_pres_cache() const {
    return this->pres_cache;
}

