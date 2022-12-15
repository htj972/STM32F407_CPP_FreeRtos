/**
* @Author kokirika
* @Name Humidity_BASE
* @Date 2022-10-20
**/

#include "Humidity_BASE.h"

Humidity_BASE::Humidity_BASE(float offset) {
    this->Humi_offset=offset;
}

float Humidity_BASE::get_humi() {
    this->Humi_cache=this->get_sensor_humi()-this->Humi_offset;
    return this->Humi_cache;
}

float Humidity_BASE::get_sensor_humi() {
    return 0;
}

void Humidity_BASE::set_humi_offset(float offset) {
    this->Humi_offset=offset;
}

float Humidity_BASE::get_humi_offset() const {
    return this->Humi_offset;
}

void Humidity_BASE::calculate_humi_offset(float temp) {
    this->Humi_offset=this->get_sensor_humi()-temp;
}

float Humidity_BASE::get_humi_cache() const {
    return this->Humi_cache;
}

