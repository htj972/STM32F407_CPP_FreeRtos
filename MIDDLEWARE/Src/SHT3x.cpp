/**
* @Author kokirika
* @Name SHT3X
* @Date 2022-10-21
**/

#include "SHT3x.h"

SHT3x::SHT3x(Software_IIC *IICx, uint8_t Add) {
    this->IIC=IICx;
    this->addr=Add;
}

void SHT3x::init(Software_IIC *IICx,uint8_t Add) {
    this->IIC=IICx;
    this->addr=Add;
    this->IIC->Queue_star();
    this->IIC->Write_One_Byte(this->addr,0x21,0x30);
    this->IIC->Queue_end();
}

bool SHT3x::init() const {
    if(this->IIC!= nullptr) {
        this->IIC->Queue_star();
        this->IIC->Write_One_Byte(this->addr, 0x21, 0x30);
        this->IIC->Queue_end();
    }
    else return false;
    return true;
}

bool SHT3x::get_sensor_data() {
    this->IIC->Queue_star();
    this->IIC->Start();
    this->IIC->Send_Byte(this->addr);
    if(!this->IIC->Wait_Ack())goto false_label;
    this->IIC->Send_Byte(0xe0);
    if(!this->IIC->Wait_Ack())goto false_label;
    this->IIC->Send_Byte(0x00);
    if(!this->IIC->Wait_Ack())goto false_label;
    this->IIC->Start();
    this->IIC->Send_Byte(this->addr+1);
    if(!this->IIC->Wait_Ack())goto false_label;
    for(uint8_t ii=0;ii<6;ii++)
        this->sht_data[ii]=this->IIC->Read_Byte((ii==6-1)?0:1);
    this->IIC->Stop();
    this->IIC->Queue_end();
    return true;
    false_label:
    this->IIC->Stop();
    this->IIC->Queue_end();
    return false;
}

int SHT3x::crc8_compute(const uint8_t *check_data) {
    uint8_t crc = 0xFF;   // calculated checksum
    for(uint8_t byteCtr = 0; byteCtr < 2; byteCtr++) {
        crc ^= (check_data[byteCtr]);
        for(uint8_t bit = 8; bit > 0; --bit) {
            if(crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            }  else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

bool SHT3x::crc8_check(uint8_t *p, uint8_t CrcData) {
    uint8_t crc;
    crc = crc8_compute(p);// calculates 8-Bit checksum
    if(crc != CrcData)
    {
        return false;
    }
    return true;
}

bool SHT3x::get_sensor_temp_humi(float* Temp,float *Humi)
{
    uint16_t data;
    this->get_sensor_data();
    if(crc8_check(this->sht_data, this->sht_data[2]))
    {
        data=((uint16_t)this->sht_data[0] << 8) | this->sht_data[1];
        * Temp =(175.0f * (float)data / 65535.0f - 45.0f);
    }
    else return false;
    if(crc8_check(&this->sht_data[3], this->sht_data[5]))
    {
        data=((uint16_t )this->sht_data[3] << 8) | this->sht_data[4];
        * Humi =(100.0f *(float)data / 65535.0f);
    }
    else return false;
    return true;
}

float SHT3x::get_sensor_temp() {
    uint16_t data;
    float Temp;
    this->get_sensor_data();
    if(crc8_check(this->sht_data, this->sht_data[2]))
    {
        data=((uint16_t)this->sht_data[0] << 8) | this->sht_data[1];
        Temp =(175.0f * (float)data / 65535.0f - 45.0f);
        return Temp;
    }
    else return 0;
}

float SHT3x::get_sensor_humi() {
    uint16_t data;
    float Humi;
    this->get_sensor_data();
    if(crc8_check(&this->sht_data[3], this->sht_data[5]))
    {
        data=((uint16_t )this->sht_data[3] << 8) | this->sht_data[4];
        Humi =(100.0f *(float)data / 65535.0f);
        return Humi;
    }
    else return 0;
}





