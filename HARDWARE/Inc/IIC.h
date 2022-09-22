/**
* @Author kokirika
* @Name IIC
* @Date 2022-09-15
**/

#ifndef Kokirika_IIC_H
#define Kokirika_IIC_H

#include "GPIO.h"

class Software_IIC {
private:
    _GPIO_ SCL;
    _GPIO_ SDA;
    enum {
        msec=1,
        usec=0,
    };
    uint16_t err_times = 200;
    uint16_t delay_time= 1;
    uint8_t dalay_mode= Software_IIC::usec;
    void SDA_config(GPIOMode_TypeDef Pin_mode);
    void SDA_OUT();
    void SDA_IN();
    void SDA_ON();
    void SDA_OFF();
    void SCL_ON();
    void SCL_OFF();
    void delay() const;
public:
    void init(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,GPIO_TypeDef *PORT_sda,uint32_t Pin_sda);
    void init(uint8_t Pin_Scl, uint8_t Pin_Sda);
    void config(uint16_t wait,uint16_t delay=1,uint8_t mode=0);
    void pin_config(GPIOOType_TypeDef Pin_mode);
    void pin_config(GPIOOType_TypeDef SCL_mode,GPIOOType_TypeDef SDA_mode);
    void set_err_time(uint16_t times);
    void set_delay(uint16_t delay,uint8_t mode);

    void Start();
    void Stop();
    uint8_t Wait_Ack();
    void Ack();
    void NAck();
    void Send_Byte(uint8_t data);
    uint8_t Read_Byte(uint8_t ack);
    void Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
    uint8_t Read_One_Byte(uint8_t daddr,uint8_t addr);

};

class HARD_IIC{
private:
    _GPIO_ SCL;
    _GPIO_ SDA;
    I2C_TypeDef* IIC;
    I2C_InitTypeDef I2C_InitStructure;
    uint8_t config_flag=0;
    uint16_t err_times = 1000;

    void default_config();

public:
    void config(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,GPIO_TypeDef *PORT_sda,uint32_t Pin_sda);
    void config(uint8_t Pin_Scl, uint8_t Pin_Sda);
    void init(I2C_TypeDef* IICx,uint8_t Address,uint32_t Speed=100000);

    void Start();
    void Stop();
    void Send_Byte_address(uint8_t daddr);
    void Read_Byte_address(uint8_t daddr);
    void Send_Byte(uint8_t data);
    uint8_t Read_Byte();

    void Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
    uint8_t Read_One_Byte(uint8_t daddr,uint8_t addr);
};


#endif //Kokirika_IIC_H






