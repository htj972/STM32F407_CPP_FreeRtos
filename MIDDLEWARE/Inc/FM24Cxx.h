/**
* @Author kokirika
* @Name FM24Cxx
* @Date 2022-10-19
**/

#ifndef KOKIRIKA_FM24CXX_H
#define KOKIRIKA_FM24CXX_H

#include "Storage_BASE.h"
#include "IIC.h"

class FM24Cxx: public Storage_BASE{
private:
    uint16_t  EE_TYPE=0;
protected:
    Software_IIC *IIC{};
public:
    enum TYPE{
        FM24C01		=127,
        FM24C02		=255,
        FM24C04		=511,
        FM24C08		=1023,
        FM24C16		=2047,
        FM24C32		=4095,
        FM24C64	    =8191,
        FM24C128	=16383,
        FM24C256	=32767,
    };
    explicit FM24Cxx(Software_IIC *IICx,uint16_t TYPE=TYPE::FM24C64);
    FM24Cxx()=default;
    ~FM24Cxx()=default;
    bool init(Software_IIC *IICx,uint16_t TYPE=TYPE::FM24C64);
    bool init();
    bool check();

    uint8_t read(uint32_t Addr) override;
    void    read(uint32_t Addr,uint8_t *data) override;
    void    read(uint32_t Addr,uint8_t *pBuffer,uint16_t Num) override;

    uint16_t write(uint32_t Addr, uint8_t Data) override;
    uint16_t write(uint32_t Addr,uint8_t *pBuffer,uint16_t Num) override;

};



#endif //KOKIRIKA_FM24CXX_H
