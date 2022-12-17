/**
* @Author kokirika
* @Name MS5805
* @Date 2022-10-25
**/

#include "MS5805.h"
#include "delay.h"

#define CMD_RESET       0x1E      // ADC reset command
#define CMD_ADC_READ    0x00      // ADC read command
#define CMD_ADC_CONV    0x40      // ADC conversion command
#define CMD_ADC_D1      0x00      // ADC D1 conversion
#define CMD_ADC_D2      0x10      // ADC D2 conversion
#define CMD_ADC_256     0x00      // ADC OSR=256
#define CMD_ADC_512     0x02      // ADC OSR=512
#define CMD_ADC_1024    0x04      // ADC OSR=1024
#define CMD_ADC_2048    0x06      // ADC OSR=2048
#define CMD_ADC_4096    0x08      // ADC OSR=4096
#define CMD_PROM_RD     0xA0      // Prom read command

#define temperature_address		0x56
#define pressure_address	    0x46

MS5805::MS5805(Software_IIC *IICx, uint8_t Addr) {
    this->IIC=IICx;
    this->add=Addr;
}

void MS5805::init(Software_IIC *IICx,uint8_t Addr) {
    this->IIC=IICx;
    this->add=Addr;
    this->cmd_reset();
}

void MS5805::init() {
    if(this->IIC!= nullptr)
        this->cmd_reset();
}

void MS5805::cmd_reset()
{
    uint8_t dads[2];
    this->IIC->Queue_star();
    uint8_t i=0;
    this->IIC->Write_One_CMD(this->add,CMD_RESET);
    for(i=0;i<8;i++)
    {
        this->register_d[i]=read_ms5805_2(CMD_PROM_RD+i*2);
    }
    this->IIC->Queue_end();
}


uint16_t MS5805::read_ms5805_2(uint8_t cmd) const
{
    uint8_t data[2];
    this->IIC->Read_Data(this->add,cmd,data,2);
    return (data[0]<<8)+(data[1]);
}

uint32_t MS5805::read_ms5805_3(uint8_t cmd) const
{
    uint8_t data[3];
    this->IIC->Read_Data(this->add,cmd,data,2);
    return (data[0]<<16)+(data[1]<<8)+(data[2]);
}

float MS5805::get_sensor_temp() {
    this->IIC->Queue_star();
    uint32_t MS_D2=0;
    this->IIC->Write_One_CMD(this->add,temperature_address);
    delay_ms(10);
    MS_D2=read_ms5805_3(CMD_ADC_READ);
    this->IIC->Queue_end();

    double dT = MS_D2-this->register_d[5]*256.0;
    return (float)(2000+(dT*this->register_d[6])/83886080)/100;
}

float MS5805::get_sensor_pres() {
    this->IIC->Queue_star();
    this->IIC->Write_One_CMD(this->add,pressure_address);
    delay_ms(10);
    uint32_t MS_D1=read_ms5805_3(CMD_ADC_READ);

    this->IIC->Write_One_CMD(this->add,temperature_address);
    delay_ms(10);
    uint32_t MS_D2=read_ms5805_3(CMD_ADC_READ);
    this->IIC->Queue_end();

    double dT   	= MS_D2-this->register_d[5]*256.0;
    double offset  = this->register_d[2]*131072.0+dT*this->register_d[4]/64.0;
    double SENS 	= this->register_d[1]*65536.0+dT*this->register_d[3]/128.0;
    return (float)(((MS_D1*SENS)/2097152.0-offset)/32768.0)/1000; //kPa
}

bool MS5805::get_sensor_temp_pres(float *Temp, float *Pres) {
    this->IIC->Queue_star();
    this->IIC->Write_One_CMD(this->add,pressure_address);
    delay_ms(10);
    uint32_t MS_D1=read_ms5805_3(CMD_ADC_READ);

    this->IIC->Write_One_CMD(this->add,temperature_address);
    delay_ms(10);
    uint32_t MS_D2=read_ms5805_3(CMD_ADC_READ);
    this->IIC->Queue_end();

    double dT   	= MS_D2-this->register_d[5]*256.0;
    double offset  = this->register_d[2]*131072.0+dT*this->register_d[4]/64.0;
    double SENS 	= this->register_d[1]*65536.0+dT*this->register_d[3]/128.0;
    *Temp = (float )(2000+(dT*this->register_d[6])/83886080)/100; //¡ãC  0.001
    *Pres = (float )(((MS_D1*SENS)/2097152.0-offset)/32768.0)/1000; //kPa
    if(*Pres>0)return true;
    else return false;
}



