/**
* @Author kokirika
* @Name OLED_SSD1306
* @Date 2022-10-15
**/

#ifndef KOKIRIKA_OLED_SSD1306_H
#define KOKIRIKA_OLED_SSD1306_H

#include "OLED_BASE.h"
#include "IIC.h"

class OLED_SSD1306:public OLED_BASE {
private:
    Software_IIC *IICX{};
    uint16_t wait_time{};
    uint8_t OWNADD{};
    bool fill_mode{};
    int X_offset{};
public:
    OLED_SSD1306()=default;
    explicit OLED_SSD1306(Software_IIC *IIC,HARD_BASE::Queue mode=HARD_BASE::Queue::OWN_Queue,int Xoffset=0,uint8_t addr=0x78);
    void config(Software_IIC *IIC,int Xoffset=0,uint8_t addr=0x78);
    bool init() override;
    void SetPos(uint16_t x, uint16_t y) override;
    void Fill(uint8_t fill_Data) override;
    void CLS() override;
    void TurnON() override;
    void TurnOFF() override;
    void WriteCmd(uint8_t Command) override;
    void WriteDat(uint8_t Data) override;
};



#endif //KOKIRIKA_OLED_SSD1306_H
