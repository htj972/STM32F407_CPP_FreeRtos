/**
* @Author kokirika
* @Name MAX31865
* @Date 2022-10-24
**/

#ifndef KOKIRIKA_MAX31865_H
#define KOKIRIKA_MAX31865_H

#include "Temperature_BASE.h"
#include "SPI.h"
#include "Out_In_Put.h"

class MAX31865: public Temperature_BASE{
public:
    enum numwires {
        _2WIRE = 0,
        _3WIRE = 1,
        _4WIRE = 0
    }; //接线模式枚举定义
    enum MODE{
        PT100 	=100,
        PT1000 	=1000,
    };
private:
    uint16_t BaudRatex{};
    uint16_t REF{};
    float RTD_A{};
    float RTD_B{};
    uint16_t modex{};
    numwires wire_num;
    void enableBias(uint8_t b);
    void writeRegister8(uint8_t addr, uint8_t data);
    uint8_t readRegister8(uint8_t addr);
    uint16_t readRegister16(uint8_t addr);
    void setWires(MAX31865::numwires wires);
    void clearFault();
    uint8_t readFault();
    uint16_t readRTD();
    void  default_config();
    void  sensor_init();
protected:
    _OutPut_ CSPin;
    SPI_BASE *spix;
public:
    MAX31865()=default;
    MAX31865(SPI_BASE *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx,numwires wires=numwires::_3WIRE,uint16_t BaudRate=SPI_BaudRatePrescaler_64);
    MAX31865(SPI_BASE *SPIx,uint8_t CSpin,numwires wires=numwires::_3WIRE,uint16_t BaudRate=SPI_BaudRatePrescaler_64);

    void init();
    void init(SPI_BASE *SPIx, GPIO_TypeDef *PORTx, uint32_t Pinx,numwires wires=numwires::_3WIRE, uint16_t BaudRate=SPI_BaudRatePrescaler_64);
    void init(SPI_BASE *SPIx,uint8_t CSpin,numwires wires=numwires::_3WIRE,uint16_t BaudRate=SPI_BaudRatePrescaler_64);
    void config(MAX31865::MODE mode,float RTDAx,float RTDBx,uint16_t ref);
    void config(MAX31865::MODE mode,uint16_t ref);
    float get_sensor_temp() override;

};


#endif //KOKIRIKA_MAX31865_H
