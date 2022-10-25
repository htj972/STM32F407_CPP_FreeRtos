/**
* @Author kokirika
* @Name TEMPERATURE_BASE
* @Date 2022-10-20
**/

#include "Temperature_BASE.h"

Temperature_BASE::Temperature_BASE(float offset) {
    this->temp_offset=offset;
}

float Temperature_BASE::get_temp() {
    return this->get_sensor_temp()-this->temp_offset;
}

float Temperature_BASE::get_sensor_temp() {
    return 0;
}

void Temperature_BASE::set_temp_offset(float offset) {
    this->temp_offset=offset;
}

float Temperature_BASE::get_temp_offset() const {
    return this->temp_offset;
}

void Temperature_BASE::calculate_temp_offset(float temp) {
    this->temp_offset=this->get_sensor_temp()-temp;
}

