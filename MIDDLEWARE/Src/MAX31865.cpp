/**
* @Author kokirika
* @Name MAX31865
* @Date 2022-10-24
**/

#include "MAX31865.h"
#include "delay.h"
#include <cmath>

#define MAX31856_CONFIG_REG            0x00
#define MAX31856_CONFIG_BIAS           0x80
#define MAX31856_CONFIG_MODEAUTO       0x40
#define MAX31856_CONFIG_MODEOFF        0x00
#define MAX31856_CONFIG_1SHOT          0x20
#define MAX31856_CONFIG_3WIRE          0x10
#define MAX31856_CONFIG_24WIRE         0x00
#define MAX31856_CONFIG_FAULTSTAT      0x02
#define MAX31856_CONFIG_FILT50HZ       0x01
#define MAX31856_CONFIG_FILT60HZ       0x00

#define MAX31856_RTDMSB_REG           0x01
#define MAX31856_RTDLSB_REG           0x02
#define MAX31856_HFAULTMSB_REG        0x03
#define MAX31856_HFAULTLSB_REG        0x04
#define MAX31856_LFAULTMSB_REG        0x05
#define MAX31856_LFAULTLSB_REG        0x06
#define MAX31856_FAULTSTAT_REG        0x07


#define MAX31865_FAULT_HIGHTHRESH     0x80
#define MAX31865_FAULT_LOWTHRESH      0x40
#define MAX31865_FAULT_REFINLOW       0x20
#define MAX31865_FAULT_REFINHIGH      0x10
#define MAX31865_FAULT_RTDINLOW       0x08
#define MAX31865_FAULT_OVUV           0x04

MAX31865::MAX31865(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx,numwires wires, uint16_t BaudRate) {
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->wire_num=wires;
    this->CSPin.init(PORTx,Pinx,GPIO_Mode_OUT);
}

MAX31865::MAX31865(SPI *SPIx, uint8_t CSpin,numwires wires, uint16_t BaudRate) {
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->wire_num=wires;
    this->CSPin.init(CSpin,GPIO_Mode_OUT);
}

void MAX31865::init() {
    if((this->spix!= nullptr)&&(this->CSPin.Pinx!=0xff))
    {
        this->CSPin.set_output(HIGH);
        this->spix->SetSpeed(this->BaudRatex);
        this->sensor_init();
        this->default_config();
    }
}

void MAX31865::init(SPI *SPIx, GPIO_TypeDef *PORTx, uint32_t Pinx,numwires wires, uint16_t BaudRate) {
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->wire_num=wires;
    this->CSPin.init(PORTx,Pinx,GPIO_Mode_OUT);
    this->CSPin.set_output(HIGH);
    this->spix->SetSpeed(BaudRate);
    this->sensor_init();
    this->default_config();
}

void MAX31865::init(SPI *SPIx,uint8_t CSpin,numwires wires,uint16_t BaudRate)
{
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->wire_num=wires;
    this->CSPin.init(CSpin,GPIO_Mode_OUT);
    this->CSPin.set_output(HIGH);
    this->spix->SetSpeed(BaudRate);
    this->sensor_init();
    this->default_config();
}

void MAX31865::default_config(){
    this->config(MAX31865::MODE::PT100);
}

void MAX31865::sensor_init() {
    this->spix->Queue_star();
    writeRegister8(0x00, 0x00);
    enableBias(1);
    delay_ms(10);
    setWires(this->wire_num);
    clearFault();
    this->spix->Queue_end();
}

void MAX31865::config(MAX31865::MODE mode, float RTDAx, float RTDBx) {
    this->modex=mode;
    this->RTD_A=RTDAx;
    this->RTD_B=RTDBx;
}

void MAX31865::config(MAX31865::MODE mode) {
    this->modex=mode;
    if(mode==MAX31865::MODE::PT100)
    {
        this->RTD_A= 3.8623e-3;
        this->RTD_B=-6.5312e-7;
    } else{
        this->RTD_A= 3.9083e-3;
        this->RTD_B=-5.775e-7;
    }
}
//使能偏执电压
void MAX31865::enableBias(uint8_t b)
{
    uint8_t t = this->readRegister8(MAX31856_CONFIG_REG);
    if (b) {
        t |= MAX31856_CONFIG_BIAS;       // enable bias
    } else {
        t &= ~MAX31856_CONFIG_BIAS;       // disable bias
    }
    this->writeRegister8(MAX31856_CONFIG_REG, t);
}
//指定寄存器写8位数据
void MAX31865::writeRegister8(uint8_t addr, uint8_t data) const
{
    addr |= 0x80;
    this->CSPin.set_output(LOW);
    this->spix->ReadWriteDATA(addr);
    this->spix->ReadWriteDATA(data);
    this->CSPin.set_output(HIGH);
}
//读寄存器，8位
uint8_t MAX31865::readRegister8(uint8_t addr) const
{
    addr &= 0x7F;
    this->CSPin.set_output(LOW);
    this->spix->ReadWriteDATA(addr);
    uint8_t ret = this->spix->ReadWriteDATA(0xff);
    this->CSPin.set_output(HIGH);
    return ret;
}
uint16_t MAX31865::readRegister16(uint8_t addr) const
{
    uint8_t buffer[2] = {0, 0};
    addr &= 0x7F;                             // make sure top bit is 0
    this->CSPin.set_output(LOW);
    this->spix->ReadWriteDATA(addr);                  //发送读取状态寄存器命令
    buffer[0] = this->spix->ReadWriteDATA(0Xff);
    buffer[1] = this->spix->ReadWriteDATA(0Xff);
    this->CSPin.set_output(HIGH);
    uint16_t ret = buffer[0];
    ret <<= 8;
    ret |=  buffer[1];
    return ret;
}

void MAX31865::setWires(MAX31865::numwires wires)
{
    uint8_t t = this->readRegister8(MAX31856_CONFIG_REG);
    if (wires == MAX31865::numwires::_3WIRE) {
        t |= MAX31856_CONFIG_3WIRE;
    } else {
        // 2 or 4 wire
        t &= ~MAX31856_CONFIG_3WIRE;
    }
    this->writeRegister8(MAX31856_CONFIG_REG, t);
}
//清除故障标志位
void MAX31865::clearFault()
{
    uint8_t t = this->readRegister8(MAX31856_CONFIG_REG);
    t &= ~0x2C;
    t |= MAX31856_CONFIG_FAULTSTAT;
    this->writeRegister8(MAX31856_CONFIG_REG, t);
}

uint8_t MAX31865::readFault()
{
    return this->readRegister8(MAX31856_FAULTSTAT_REG);
}
//RTD数据读取
uint16_t MAX31865::readRTD()
{
    this->spix->Queue_star();
    uint8_t t = this->readRegister8(MAX31856_CONFIG_REG);
    t |= MAX31856_CONFIG_1SHOT;
    this->writeRegister8(MAX31856_CONFIG_REG, t);
    delay_ms(70);                                   //单次转换读取时间在片选信号拉高后在50HZ工作模式下需要约62ms，60hz约52ms
    uint16_t rtd = this->readRegister16(MAX31856_RTDMSB_REG);
    // remove fault
    rtd >>= 1;
    this->spix->Queue_end();
    return rtd;
}
//温度计算
float MAX31865::get_sensor_temp()
{
    float Z1, Z2, Z3, Z4, Rt;
    float	temp,rpoly;
    Rt = this->readRTD();

    Rt=(float)Rt/32767.0f*(float)this->REF;

    Z1 = -RTD_A;
    Z2 = RTD_A * RTD_A - (4 * RTD_B);
    Z3 = (4 * RTD_B) / (float)this->modex;
    Z4 = 2 * RTD_B;

    temp = Z2 + (Z3 * Rt);
    temp = ((float)sqrt((double)temp) + Z1) / Z4;

    if (temp >= 0) return temp;
    rpoly = Rt;

    temp = -242.02f;
    temp += 2.2228f * rpoly;
    rpoly *= Rt;  // square
    temp += 2.5859e-3f * rpoly;
    rpoly *= Rt;  // ^3
    temp -= 4.8260e-6f * rpoly;
    rpoly *= Rt;  // ^4
    temp -= 2.8183e-8f * rpoly;
    rpoly *= Rt;  // ^5
    temp += 1.5243e-10f * rpoly;

    return temp;
}




