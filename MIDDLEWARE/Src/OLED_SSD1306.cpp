/**
* @Author kokirika
* @Name Project
* @Date 2022-10-15
**/

#include "../BASE_CLASS/Inc/OLED_SSD1306.h"

OLED_SSD1306::OLED_SSD1306(Software_IIC *IIC,int Xoffset,uint8_t addr) {
    this->config(IIC,Xoffset,addr);
}

void OLED_SSD1306::config(Software_IIC *IIC,int Xoffset,uint8_t addr) {
    if(IIC!= nullptr)
        this->IICX=IIC;
    this->OWNADD=addr;
    this->X_offset=Xoffset;
}

bool OLED_SSD1306::init() {
    this->IICX->Start();
    this->IICX->Send_Byte(this->OWNADD);
    while(this->IICX->Wait_Ack()&&this->wait_time--);
    this->IICX->Stop();
    if(this->wait_time<1)return false;
    this->WriteCmd(0xAE); //display off
    this->WriteCmd(0x20); //Set Memory Addressing Mode
    this->WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    this->WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    this->WriteCmd(0xc8); //Set COM Output Scan Direction
    this->WriteCmd(0x00); //---set low column address
    this->WriteCmd(0x10); //---set high column address
    this->WriteCmd(0x40); //--set start line address
    this->WriteCmd(0x81); //--set contrast control register
    this->WriteCmd(0xff); //ÁÁ¶Èµ÷½Ú 0x00~0xff
    this->WriteCmd(0xa1); //--set segment re-map 0 to 127
    this->WriteCmd(0xa6); //--set normal display
    this->WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    this->WriteCmd(0x3F); //
    this->WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    this->WriteCmd(0xd3); //-set display offset
    this->WriteCmd(0x00); //-not offset
    this->WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    this->WriteCmd(0xf0); //--set divide ratio
    this->WriteCmd(0xd9); //--set pre-charge period
    this->WriteCmd(0x22); //
    this->WriteCmd(0xda); //--set com pins hardware configuration
    this->WriteCmd(0x12);
    this->WriteCmd(0xdb); //--set vcomh
    this->WriteCmd(0x20); //0x20,0.77xVcc
    this->WriteCmd(0x8d); //--set DC-DC enable
    this->WriteCmd(0x14); //
    this->WriteCmd(0xaf); //--turn on oled panel
    return true;
}

void OLED_SSD1306::SetPos(uint16_t x, uint16_t y) {
    this->WriteCmd(0xb0+y);
    this->WriteCmd(((x&0xf0)>>4)|0x10);
    this->WriteCmd((x&0x0f)|0x01);
}

void OLED_SSD1306::Fill(uint8_t fill_Data) {
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        this->SetPos(0,m);
        for(n=0;n<128+this->X_offset;n++)
        {
            this->WriteDat(fill_Data);
        }
    }
}

void OLED_SSD1306::CLS() {
    this->Fill(0x00);
}

void OLED_SSD1306::TurnON() {
    this->WriteCmd(0X8D);  //ÉèÖÃµçºÉ±Ã
    this->WriteCmd(0X14);  //¿ªÆôµçºÉ±Ã
    this->WriteCmd(0XAF);  //OLED»½ÐÑ
}

void OLED_SSD1306::TurnOFF() {
    this->WriteCmd(0X8D);  //ÉèÖÃµçºÉ±Ã
    this->WriteCmd(0X10);  //¹Ø±ÕµçºÉ±Ã
    this->WriteCmd(0XAE);  //OLEDÐÝÃß
}

void OLED_SSD1306::WriteCmd(uint8_t Command) {
    this->IICX->Write_One_Byte(this->OWNADD,0x00,Command);
}

void OLED_SSD1306::WriteDat(uint8_t Data) {
    this->IICX->Write_One_Byte(this->OWNADD,0x40,Data);
}




