/**
* @Author kokirika
* @Name DS18B20
* @Date 2022-10-26
**/

#ifndef KOKIRIKA_DS18B20_H
#define KOKIRIKA_DS18B20_H

#include "ONE_Wire.h"
#include "Temperature_BASE.h"

class DS18B20: public ONE_Wire,public Temperature_BASE{
private:
    void Rst();
    uint8_t Read_Bit();
    bool default_init();
    void Start();
    void Write_Byte(uint8_t dat);
public:
    explicit DS18B20(uint8_t Pinx,uint16_t err_t=200);
    DS18B20(GPIO_TypeDef *PORT,uint32_t Pinx,uint16_t err_t=200);
    DS18B20();
    ~DS18B20()=default;
    void init();
    void init(uint8_t Pinx,uint16_t err_t=200);
    void init(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,uint16_t err_t=200);
    uint8_t Read_Byte();

    float get_sensor_temp() override;
};


#endif //KOKIRIKA_DS18B20_H
