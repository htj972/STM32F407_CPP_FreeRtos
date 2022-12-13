/**
* @Author kokirika
* @Name DW_LCD
* @Date 2022-12-09
**/

#ifndef KOKIRIKA_DW_LCD_H
#define KOKIRIKA_DW_LCD_H

#include "USART.h"
#include "RTCBASE.h"
#include "Timer.h"

#define  pic_H_address  0X3300
#define  key_H_address  0X3000
#define text_H_address  0X1000
#define  point_address  0x2000

#define PIC_ADD(n) (pic_H_address+0x20*((n)-1))

class DW_LCD: public HARD_BASE,public Call_Back{
protected:
    _USART_ *USARTX{};
    RTCBASE *RTX{};
    Timer   *timer{};
private:
    uint8_t head_address[2]{};
    uint8_t curInterface{};
    uint8_t cur_light{};
    uint8_t fifo_data[16]{};
    uint8_t uart_get_len{};
    uint8_t uart_get_data{};
    uint16_t ret_key_address{};
    uint16_t ret_key_data{};
    bool     ret_key{};
    static uint8_t hex_to_int(uint8_t hex);
    void key_back_value_point();
    void register_back_value_point();
public:
    DW_LCD()=default;
    explicit DW_LCD(_USART_ *USART,uint8_t head1=0x5A,uint8_t head2=0xA5);
    void init(_USART_ *USART,uint8_t head1=0x5A,uint8_t head2=0xA5);
    void Interface_switching(uint16_t Interface,bool save=true);
    void Data_Output(uint16_t address,float date,uint8_t decimal);
    int vspf_Text(uint16_t address,char *fmt, ...);
    void Text_Output(uint16_t address,char *p);
    void Picture_cut(uint16_t address,uint16_t PictureID,uint16_t IDs_x,uint16_t IDs_y,uint16_t IDe_x,uint16_t IDe_y,uint16_t x,uint16_t y);
    void set_Progress_bar(uint16_t address,uint8_t progress,uint16_t IDs_x,uint16_t IDs_y,uint16_t IDe_x,uint16_t IDe_y);
    void ICO_cut(uint16_t address,uint16_t IDNum);
    void Keyboard_Up(uint8_t InputNum);
    void RTC_Read();
    void RTC_write(uint8_t yer,uint8_t mon,uint8_t day,uint8_t hou,uint8_t min,uint8_t sec);
    void SetBackLight(uint8_t light,bool save=true);
    void Sleep(uint8_t Interface=0);
    void Wake_up();
    void DrawCurve(uint8_t ch, uint16_t X_scale, uint16_t Y_scale, int Y_offset, uint16_t data_num, uint16_t max_data, const uint16_t * curve_data);
    void Draw(uint8_t ch, uint16_t data_len,const uint16_t * curve_data);
    void DROW_point(uint8_t ch,uint16_t data);
    void DROW_point_set(uint8_t ch,uint8_t len,uint16_t data);
    void Write_data(uint16_t point,uint8_t offset,uint16_t DATA);

    void RtC_link(RTCBASE *RTCX);
    void Timer_Link(Timer   *timerx);

    bool available();
    void setup();

    uint16_t get_key_address();
    uint16_t get_key_data();
    bool get_key_sata();


    void Callback(int num ,char** data) override;

};


#endif //KOKIRIKA_DW_LCD_H
