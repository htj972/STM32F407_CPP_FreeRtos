/**
* @Author kokirika
* @Name OLED
* @Date 2022-10-15
**/

#ifndef KOKIRIKA_OLED_H
#define KOKIRIKA_OLED_H

#include "sys.h"
#include "string"
#include "HARD_BASE.h"
#include "Font.h"


class OLED_BASE: public HARD_BASE {

public:
    explicit OLED_BASE(HARD_BASE::Queue mode=HARD_BASE::Queue::OWN_Queue);
    virtual bool init();
    virtual void SetPos(uint16_t x, uint16_t y);
    virtual void Fill(uint8_t fill_Data);
    virtual void CLS();
    virtual void TurnON();
    virtual void TurnOFF();
    virtual void WriteCmd(uint8_t Command);
    virtual void WriteDat(uint8_t Data);
    void ShowCHR(uint8_t x,uint8_t y,char ch,uint8_t TextSize);
    void SHOW_CN_str(uint8_t x, uint8_t y,const char *ch);
    uint16_t Print(uint8_t x,uint8_t y,const char *fmt, ...);
    void Print(uint8_t x, uint8_t y, const std::string& String);
    void Print(uint8_t x, uint8_t y,const char *s);
    void Print(uint8_t x, uint8_t y,char *s);
    void Print(uint8_t x, uint8_t y,uint8_t  *s);
    void Print(uint8_t x, uint8_t y,int integer);
    void set_font_library(bool(*fun)(uint8_t *,uint8_t *));

private:
    bool (*check_font_library)(uint8_t *ZH_code,uint8_t *ZH_font){};
    void OLED_ShowCN(uint8_t x, uint8_t y,uint8_t *ch);
};


#endif //KOKIRIKA_OLED_H
