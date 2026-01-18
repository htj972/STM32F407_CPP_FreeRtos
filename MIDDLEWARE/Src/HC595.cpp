/**
* @Author kokirika
* @Name 74HC595
* @Date 2026-01-17
**/

#include <cstring>
#include "HC595.h"
#include "delay.h"

HC595::HC595(GPIO_Pin en, GPIO_Pin clk, GPIO_Pin rck, GPIO_Pin dio, GPIO_Pin clr,uint8_t len):len(len) {
    this->en.init(en,LOW);
    this->clk.init(clk,HIGH);
    this->rck.init(rck,HIGH);
    this->dio.init(dio,HIGH);
    this->clr.init(clr,LOW);
}

void HC595::init() {
    if(this->Output_data == nullptr)
    {
        this->Output_data=new uint8_t[this->len];
        this->HC_change = new uint8_t[len][8];
        for (int i = 0; i < len; ++i) {
            for (int j = 0; j < 8; ++j) {
                this->HC_change[i][j]=j;
            }
        }
    }

    this->clear(false);
    this->set_en(true);
}

void HC595::set_shift(const char data[][8]) {
    for(uint8_t i=0;i<this->len;i++)
    {
        for(uint8_t j=0;j<8;j++)
        {
            this->HC_change[i][j]=data[i][j];
        }
    }
}

void HC595::set_shift(uint8_t num, const char *data) {
    if(num>=this->len)return;
    for(uint8_t j=0;j<8;j++)
    {
        this->HC_change[num][j]=data[j];
    }
}

void HC595::set_en(bool onoff) {
    this->en.set(onoff?ON:OFF);
}

void HC595::clear(bool dis) {
    this->clr.set( ON);
    for (int i = 0; i < len; ++i) {
        this->Output_data[i]=0x00;
    }
    delay_ms(1);
    this->clr.set( OFF);
    if(dis)this->upset();
}

void HC595::upset() {
    BitAction paragraph;
    for(uint8_t j=0;j<len;j++)
    {
        for(unsigned char i : HC_change[j])
        {
            paragraph=((this->Output_data[j]>>i)&0x01)==0x01?Bit_SET:Bit_RESET;
            this->dio.set(paragraph);

            this->rck.set( HIGH);
            this->rck.set( LOW);
        }
    }
    this->clk.set( HIGH);
    this->clk.set( LOW);
}

void HC595::Set_Hex(const uint8_t *data) {
    for (int i = 0; i < len; ++i) {
        this->Output_data[i]=data[i];
    }
    this->upset();
}

void HC595::Set_output(uint8_t num, uint8_t onoff) {
    if(num>=len*8)return;
    uint8_t site=num/8;
    uint8_t data=0;
    if(onoff)
    {
        data=1<<(num%8);
        this->Output_data[site]|=data;
    }
    else
    {
        data=~(1<<(num%8));
        this->Output_data[site]&=data;
    }
    this->upset();
}

void HC595::Set_on(uint8_t num) {
    this->Set_output(num,1);
}

void HC595::Set_off(uint8_t num) {
    this->Set_output(num,0);
}

void HC595::Toggle(uint8_t num) {
    if(num>=len*8)return;
    uint8_t site=num/8;
    uint8_t data=1<<num%8;
    this->Output_data[site]^=data;
    this->upset();
}

void HC595::Get_output(uint8_t *data) {
    if(this->Output_data == nullptr)
    {
        this->init();
    }
    for(uint8_t i=0;i<this->len;i++)
    {
        data[i]=this->Output_data[i];
    }
}

bool HC595::Get_output_bit(uint8_t num) {
    if(this->Output_data == nullptr)
    {
        this->init();
    }
    if(num>=len*8)return false;
    return (this->Output_data[num/8]>>(num%8))&0x01;
}

HC595::~HC595() {
    delete[] this->Output_data;
}





