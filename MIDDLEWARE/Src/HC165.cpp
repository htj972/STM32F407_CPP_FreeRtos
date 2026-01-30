/**
* @Author kokirika
* @Name 74HC165
* @Date 2026-01-17
**/

#include <cstring>
#include "HC165.h"
#include "delay.h"

HC165::HC165(GPIO_Pin CE, GPIO_Pin CLK, GPIO_Pin SHLD, GPIO_Pin SO, uint8_t len):len(len) {
    this->CLK.init(CLK,HIGH);
    this->SHLD.init(SHLD,LOW);
    this->CE.init(CE,LOW);
    this->SO.init(SO,HIGH);
}

void HC165::init() {
    if(this->Input_data == nullptr)
    {
        this->Input_data=new uint8_t[this->len];
        this->HC_change = new uint8_t[len][8];
        for (int i = 0; i < len; ++i) {
            for (int j = 0; j < 8; ++j) {
                this->HC_change[i][j]=j;
            }
        }
    }
    this->set_en(true);
    this->clear();
}

void HC165::set_shift(const char data[][8]) {
    for(uint8_t i=0;i<this->len;i++)
    {
        for(uint8_t j=0;j<8;j++)
        {
            this->HC_change[i][j]=data[i][j];
        }
    }
}

void HC165::set_shift(uint8_t num, const char *data) {
    if(num>=this->len)return;
    for(uint8_t j=0;j<8;j++)
    {
        this->HC_change[num][j]=data[j];
    }
}

void HC165::set_en(bool onoff) {
    this->CE.set(onoff?ON:OFF);
}

void HC165::clear() {
    for (int i = 0; i < len; ++i) {
        this->Input_data[i]=0x00;
    }
}

void HC165::upset() {
    this->SHLD.set(ON);
    this->SHLD.set(OFF);
    uint8_t data;
    for(uint8_t i=0;i<this->len;i++)
    {
        data=0;
        for(unsigned char j : this->HC_change[i])
        {
            if(this->SO.get_state())
            {
                data|=(0x01<<j);
            }
            this->CLK.set(ON);
            this->CLK.set(OFF);
        }
        this->Input_data[i]=~data;
    }
}

void HC165::Get_input(uint8_t *data) {
    for(uint8_t i=0;i<this->len;i++)
    {
        data[i]=this->Input_data[i];
    }
}

void HC165::Get_input_bits(uint8_t* data)
{
    for(uint8_t i=0;i<this->len*8;i++)
    {
        data[i]=(this->Input_data[i/8]>>(i%8))&0x01;
    }
}

bool HC165::Get_input_bit(uint8_t num) {
    if(num>=len*8)return false;
    return (this->Input_data[num/8]>>(num%8))&0x01;
}

HC165::~HC165() {
    delete[] this->Input_data;
    delete[] HC_change;
}







