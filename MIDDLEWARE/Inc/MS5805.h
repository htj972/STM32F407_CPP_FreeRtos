/**
* @Author kokirika
* @Name MS5805
* @Date 2022-10-25
**/

#ifndef KOKIRIKA_MS5805_H
#define KOKIRIKA_MS5805_H

#include "Temp_Pres_BASE.h"
#include "IIC.h"

class MS5805: public Temp_Pres_BASE{
private:
    uint8_t add{};
    void cmd_reset();
    uint16_t register_d[8]{};
    uint16_t read_ms5805_2(uint8_t cmd) const;
    uint32_t read_ms5805_3(uint8_t cmd) const;
public:
    Software_IIC *IIC{};

    explicit MS5805(Software_IIC *IICx,uint8_t Addr=0xec);
    MS5805()=default;
    ~MS5805()=default;
    void init(Software_IIC *IICx,uint8_t Addr=0xec);
    void init();

    float get_sensor_temp() override;
    float get_sensor_pres() override;
    bool get_sensor_temp_pres(float* Temp,float *Pres) override;
};


#endif //KOKIRIKA_MS5805_H
