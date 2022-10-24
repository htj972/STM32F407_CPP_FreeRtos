/**
* @Author kokirika
* @Name MAX31865
* @Date 2022-10-24
**/

#ifndef KOKIRIKA_MAX31865_H
#define KOKIRIKA_MAX31865_H

#include "Temperature_BASE.h"
#include "SPI.h"

class MAX31865: public Temperature_BASE{
private:
    uint16_t BaudRatex{};
    uint16_t REF;
    float RTD_A;
    float RTD_B;
    uint16_t modex;
    void enableBias(uint8_t b);
    void writeRegister8(uint8_t addr, uint8_t data) const;
    uint8_t readRegister8(uint8_t addr) const;
    uint16_t readRegister16(uint8_t addr) const;
    void clearFault();
    uint16_t readRTD();
    void  default_config();
public:
    _GPIO_ CSPin;
    SPI *spix;
    enum numwires {
        MAX31865_2WIRE = 0,
        MAX31865_3WIRE = 1,
        MAX31865_4WIRE = 0
    }; //接线模式枚举定义
    enum MODE{
        PT100 	=100,
        PT1000 	=1000,
    };

    MAX31865(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx, uint16_t BaudRate);
    MAX31865(SPI *SPIx, uint8_t CSpin, uint16_t BaudRate);
    void init();
    void init(SPI *SPIx, GPIO_TypeDef *PORTx, uint32_t Pinx, uint16_t BaudRate);
    void init(SPI *SPIx,uint8_t CSpin,uint16_t BaudRate);
    void config(MAX31865::MODE mode,float RTDAx,float RTDBx);
    void config(MAX31865::MODE mode);
    void setWires(MAX31865::numwires wires);
    float get_sensor_temp() override;

};


#endif //KOKIRIKA_MAX31865_H
