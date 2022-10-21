/**
* @Author kokirika
* @Name SHT3X
* @Date 2022-10-21
**/

#ifndef KOKIRIKA_SHT3X_H
#define KOKIRIKA_SHT3X_H

#include "Temp_Humi_BASE.h"
#include "IIC.h"

class SHT3x: public Temp_Humi_BASE{
private:
    uint8_t addr{};
    uint8_t sht_data[6]{};
    static int crc8_compute(const uint8_t *check_data);
    static bool crc8_check(uint8_t *p, uint8_t CrcData);
public:
    Software_IIC *IIC{};

    explicit SHT3x(Software_IIC *IICx,uint8_t Add=0x88);
    SHT3x()=default;
    ~SHT3x()=default;
    void init(Software_IIC *IICx,uint8_t Add=0x88);
    bool init() const;

    bool get_sensor_data();
    bool get_sensor_temp_humi(float* Temp,float *Humi) override;
    float get_sensor_temp() override;
    float get_sensor_humi() override;
};


#endif //KOKIRIKA_SHT3X_H
