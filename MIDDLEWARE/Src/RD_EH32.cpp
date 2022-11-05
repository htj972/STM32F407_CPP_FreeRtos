/**
* @Author kokirika
* @Name RD_EH32
* @Date 2022-10-27
**/

#include "RD_EH32.h"
#include <cstdarg>

RD_EH32::RD_EH32(_USART_ *UARTx) {
    this->init(UARTx);
}

void RD_EH32::Reset() {
    unsigned char str[2]={0x1B,0x40};
    this->write(str,2);
}

void RD_EH32::Next_Page() {
    this->write(0x0C);
}

void RD_EH32::Enter() {
    this->write(0x0A);
}

void RD_EH32::Move(float mm) {
    unsigned char str[3]={0x1B,0x4A,0x04};
    auto n= (uint8_t)(mm/0.125);
    str[2]=(n>240)?240:n;
    this->write(str,3);
}

void RD_EH32::Moveline(uint8_t line) {
    unsigned char str[3]={0x1B,0x64,line};
    this->write(str,3);
}

void RD_EH32::Reverse(bool sata) {
    unsigned char str[3]={0x1B,0x63,!sata};
    this->write(str,3);
}

void RD_EH32::Hori_table(int n,...) {
    unsigned char str[32]={0x1B,0x44};
    for(auto & ii : str)
        ii=0;
    uint8_t data_num=0;
    va_list arg_ptr;
    int nArgValue;
    va_start(arg_ptr, n);
    str[2+data_num++]=n;
    do
    {
        nArgValue = va_arg(arg_ptr, int);
        if(nArgValue<0)nArgValue=0;
        else str[2+data_num++]=nArgValue;
        if(data_num>32-3-1)break;
    } while (nArgValue != 0);
    str[2+data_num]=0;
    va_end(arg_ptr);
    this->write(str,data_num+3);
}

void RD_EH32::Hori_Next_table() {
    this->write(0x09);
}

void RD_EH32::set_Overline(bool IS) {
    unsigned char str[3]={0x1B,0x2D,IS};
    this->write (str,3);
}

void RD_EH32::set_underline(bool IS) {
    unsigned char str[3]={0x1B,0x2E,IS};
    this->write (str,3);
}

void RD_EH32::Reverse_color(bool IS) {
    unsigned char str[3]={0x1D,0x42,IS};
    this->write(str, 3);
}

void RD_EH32::set_Rotation(ANGLE angle) {
    unsigned char str[3]={0x1C,0x49,angle};
    this->write(str, 3);
}

void RD_EH32::set_Site(uint16_t site) {
    if(site>384)site=384;
    uint8_t nH=(site>>8)&0xff;
    uint8_t nL=(site>>0)&0xff;
    unsigned char str[4]={0x1C,0x49,nL,nH};
    this->write(str, 4);
}

void RD_EH32::set_notprint_Lweight(uint8_t Byte) {
    unsigned char str[3]={0x1C,0x49,Byte};
    this->write(str, 3);
}

void RD_EH32::set_notprint_Rweight(uint8_t Byte) {
    unsigned char str[3]={0x1B,0x51,Byte};
    this->write(str, 3);
}

void RD_EH32::set_Row_spacing(uint8_t point) {
    unsigned char str[3]={0x1B,0x31,point};
    this->write(str, 3);
}

void RD_EH32::set_Word_spacing(uint8_t point) {
    unsigned char str[3]={0x1B,0x20,point};
    this->write(str, 3);
}

void RD_EH32::set_Alignment(RD_EH32::ALIGN mode) {
    if(mode<0x10) {
        unsigned char str[3] = {0x1B, 0x61, mode};
        this->write(str, 3);
    }
    else{
        uint8_t nH=((mode>>4)&0x0f)-1;
        unsigned char str[3] = {0x1C, 0x72, nH};
        this->write(str, 3);

        uint8_t nL=mode&0x0f-1;
        if(nL>0) {
            unsigned char str1[3] = {0x1B, 0x61, nL};
            this->write(str1, 3);
        }
    }
}

void RD_EH32::set_Vertical_magnification(uint8_t Mult) {
    if(Mult>8)Mult=8;
    unsigned char str[3]={0x1B,0x55,Mult};
    this->write(str, 3);
}

void RD_EH32::set_Horizontal_magnification(uint8_t Mult) {
    if(Mult>8)Mult=8;
    unsigned char str[3]={0x1B,0x56,Mult};
    this->write(str, 3);
}

void RD_EH32::set_magnification(uint8_t Multx,uint8_t Multy) {
    if(Multx>8)Multx=8;
    if(Multy>8)Multy=8;
    unsigned char str[4]={0x1B,0x58,Multx,Multy};
    this->write(str, 4);
}

void RD_EH32::Drow_pic1() {
    unsigned char str[30];
    unsigned char i=0;
    str[i++] = 0x1B;
    str[i++] = 0x4B;
    str[i++] = 15; //打印15个点宽图形19 / 45
    str[i++] = 0;
    str[i++] = 0x7C; str[i++] = 0x44; str[i++] = 0x44; str[i++] = 0xFF;
    str[i++] = 0x44; str[i++] = 0x44; str[i++] = 0x7C; str[i++] = 0x00;
    str[i++] = 0x41; str[i++] = 0x62; str[i++] = 0x54; str[i++] = 0xC8;
    str[i++] = 0x54; str[i++] = 0x62; str[i++] = 0x41; str[i++] = 0x0D;
    this->write(str,i);//发送图形打印命令。
}

void RD_EH32::Drow_pic2() {
    unsigned char str[200];
    unsigned char j=0;
    str[j++] = 0x1B;
    str[j++] = 0x2A;
    str[j++] = 32; //m=32(高度 24 点、倍宽)
    str[j++] = 12; //图象宽度为 12dots
    str[j++] = 0;
//位图数据
    str[j++] = 0x10;str[j++] = 0x00;str[j++] = 0x20;str[j++] = 0x1F;str[j++] = 0xFF;str[j++] =0xE0;
    str[j++] = 0x1F;str[j++] = 0xFF;str[j++] = 0xE0;str[j++] = 0x10;str[j++] = 0x20;str[j++] =0x20;
    str[j++] = 0x10;str[j++] = 0x20;str[j++] = 0x00;str[j++] = 0x10;str[j++] = 0x30;str[j++] =0x00;
    str[j++] = 0x10;str[j++] = 0x3C;str[j++] = 0x00;str[j++] = 0x10;str[j++] = 0x2f;str[j++] =0x00;
    str[j++] = 0x18;str[j++] = 0x43;str[j++] = 0xC0;str[j++] = 0x0F;str[j++] = 0xC0;str[j++] =0xE0;
    str[j++] = 0x07;str[j++] = 0x80;str[j++] = 0x20;str[j++] = 0x00;str[j++] = 0x00;str[j++] =0x20;
    str[j++] = 0x0D;//打印出当前的图形
    this->write(str,j);
}

void RD_EH32::Drow_Raster_bitmap() {
    unsigned char str[200];
    unsigned char j=0;
    str[j++] = 0x1D;
    str[j++] = 0x76;
    str[j++] = 30;

    str[j++] = 51;

    str[j++] = 6;
    str[j++] = 0;
    str[j++] = 6;
    str[j++] = 0;
//位图数据
    str[j++] = 0x10;str[j++] = 0x00;str[j++] = 0x20;str[j++] = 0x1F;str[j++] = 0xFF;str[j++] =0xE0;
    str[j++] = 0x1F;str[j++] = 0xFF;str[j++] = 0xE0;str[j++] = 0x10;str[j++] = 0x20;str[j++] =0x20;
    str[j++] = 0x10;str[j++] = 0x20;str[j++] = 0x00;str[j++] = 0x10;str[j++] = 0x30;str[j++] =0x00;
    str[j++] = 0x10;str[j++] = 0x3C;str[j++] = 0x00;str[j++] = 0x10;str[j++] = 0x2f;str[j++] =0x00;
    str[j++] = 0x18;str[j++] = 0x43;str[j++] = 0xC0;str[j++] = 0x0F;str[j++] = 0xC0;str[j++] =0xE0;
    str[j++] = 0x07;str[j++] = 0x80;str[j++] = 0x20;str[j++] = 0x00;str[j++] = 0x00;str[j++] =0x20;
    str[j++] = 0x0D;//打印出当前的图形
    this->write(str,j);
}


