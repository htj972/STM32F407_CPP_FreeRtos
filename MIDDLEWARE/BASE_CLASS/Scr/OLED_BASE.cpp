/**
* @Author kokirika
* @Name Project
* @Date 2022-10-15
**/

#include "OLED_BASE.h"
#include <cstdarg>

OLED_BASE::OLED_BASE(HARD_BASE::Queue mode) {
    this->set_Queue_mode(mode);
}

void OLED_BASE::ShowCHR(uint8_t x, uint8_t y, char ch, uint8_t TextSize) {
    unsigned char c,i;
    switch(TextSize)
    {
        case 16:case 1:
        {
            c = ch - 32;
            if(x > 126)
            {
                x = 0;
                y++;
            }
            this->SetPos(x,y);
            for(i=0;i<6;i++)
                this->WriteDat(kokirika_F6x8[c][i]);
        }break;
        case 32:case 2:
        {
            c = ch - 32;
            if(x > 120)
            {
                x = 0;
                y++;
            }
            this->SetPos(x,y);
            for(i=0;i<8;i++)
                this->WriteDat(kokirika_F8X16[c*16+i]);
            this->SetPos(x,y+1);
            for(i=0;i<8;i++)
                this->WriteDat(kokirika_F8X16[c*16+i+8]);
        }break;
        default:break;
    }
}

void OLED_BASE::OLED_ShowCN(uint8_t x, uint8_t y, uint8_t *ch)
{
    unsigned char wm;
    this->SetPos(x , y);
    for(wm = 0;wm < 16;wm++)
        WriteDat(ch[wm]);
    this->SetPos(x,y + 1);
    for(wm = 0;wm < 16;wm++)
        WriteDat(ch[wm+16]);
}

void OLED_BASE::SHOW_CN_str(uint8_t x, uint8_t y,const char *ch) {
    uint8_t j=0;
    while(ch[j] != '\0')
    {
        if(x>111) {y+=2;x=0;}
        if(ch[j]<127)
        {
            this->ShowCHR(x,y,ch[j],2);
            if(ch[j]=='m')x+=2;
            j++;
            x+=7+1;
        }
        else
        {
            uint8_t dis_temp[32];
            uint8_t ch_temp[2];
            ch_temp[0]=ch[j];
            ch_temp[1]=ch[j+1];
            if(this->check_font_library!=nullptr){
                if (this->check_font_library(ch_temp, dis_temp))
                    OLED_ShowCN(x, y, dis_temp);
            }
            j+=2;
            x+=16+1;
        }
    }
}

uint16_t OLED_BASE::Print(uint8_t x,uint8_t y,const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    std::string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    this->Print(x,y,r);
    return r.length();
}

void OLED_BASE::Print(uint8_t x, uint8_t y, const std::string& String) {
    const char* p=String.c_str();
    this->SHOW_CN_str(x,y,p);
}

void OLED_BASE::Print(uint8_t x, uint8_t y,const char *s) {
    this->Print(x,y,"%s",s);
}

void OLED_BASE::Print(uint8_t x, uint8_t y,char *s) {
    this->Print(x,y,"%s",s);
}

void OLED_BASE::Print(uint8_t x, uint8_t y,int integer) {
    this->Print(x,y,"%d",integer);
}

void OLED_BASE::set_font_library(bool (*fun)(uint8_t *, uint8_t *)) {
    this->check_font_library=fun;
}

bool OLED_BASE::init() {
    return false;
}

void OLED_BASE::SetPos(uint16_t x, uint16_t y) {

}

void OLED_BASE::Fill(uint8_t fill_Data) {

}

void OLED_BASE::CLS() {

}

void OLED_BASE::TurnON() {

}

void OLED_BASE::TurnOFF() {

}

void OLED_BASE::WriteCmd(uint8_t Command) {

}

void OLED_BASE::WriteDat(uint8_t Data) {

}

