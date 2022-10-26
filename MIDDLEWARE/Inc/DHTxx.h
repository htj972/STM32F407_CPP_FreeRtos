/**
* @Author kokirika
* @Name DHTXX
* @Date 2022-10-25
**/

#ifndef KOKIRIKA_DHTXX_H
#define KOKIRIKA_DHTXX_H

#include "ONE_Wire.h"
#include "Temp_Humi_BASE.h"

class DHTxx: public ONE_Wire,public Temp_Humi_BASE{
private:
    void Rst();
    uint8_t Read_Bit();
    bool default_init();
public:
    explicit DHTxx(uint8_t Pinx,uint16_t err_t=200);
    DHTxx(GPIO_TypeDef *PORT,uint32_t Pinx,uint16_t err_t=200);
    DHTxx();
    ~DHTxx()=default;
    void init();
    void init(uint8_t Pinx,uint16_t err_t=200);
    void init(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,uint16_t err_t=200);
    uint8_t Read_Byte();

    bool get_sensor_temp_humi(float* Temp,float *Humi) override;
    float get_sensor_temp() override;
    float get_sensor_humi() override;
};


#endif //KOKIRIKA_DHTXX_H
