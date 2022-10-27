/**
* @Author kokirika
* @Name RD_EH32
* @Date 2022-10-27
**/

#ifndef KOKIRIKA_RD_EH32_H
#define KOKIRIKA_RD_EH32_H

#include "Printer_usart_BASE.h"

class RD_EH32: public Printer_usart_BASE{
public:
    enum ANGLE:uint8_t{
        _0   = 0,
        _90  = 1,
        _180 = 2,
        _270 = 3,
    };
    enum ALIGN:uint8_t{
        _left   =0,
        _middle =1,
        _right  =2,
        _up     =0x10,
        _down   =0x20,
    };
private:

public:
    explicit RD_EH32(_USART_ *UARTx);
    void Reset();                               //复位
    void Next_Page();                           //翻页
    void Enter();                               //打印缓存
    void Move(float mm=0.5);                    //出纸距离
    void Moveline(uint8_t line=1);              //出纸行数
    void Reverse(bool sata=true);               //反转打印
    void Hori_table(int n,...);                 //水平制表
    void Hori_Next_table();                     //移动制表
    void set_Overline(bool IS=true);            //上划线
    void set_underline(bool IS=true);           //下划线
    void Reverse_color(bool IS=true);           //颜色反转
    void set_Rotation(ANGLE angle=ANGLE::_0);   //字符旋转
    void set_Site(uint16_t site);               //设置绝对位置
    void set_notprint_Lweight(uint8_t Byte);    //左侧禁止打印
    void set_notprint_Rweight(uint8_t Byte);    //右侧禁止打印
    void set_Row_spacing(uint8_t point);        //设置行间距
    void set_Word_spacing(uint8_t point);       //设置字间距
    void set_Alignment(ALIGN mode);             //设置对齐
    void set_Vertical_magnification(uint8_t Mult);//水平放大1~8
    void set_Horizontal_magnification(uint8_t Mult);//垂直放大1~8
    void set_magnification(uint8_t Multx,uint8_t Multy);//字符放大1~8 水平 垂直

    void Drow_pic1();                             //打印图片 可以放大
    void Drow_pic2();                             //打印图片  不可以放大
    void Drow_Raster_bitmap();                    //打印光栅位图
};




#endif //KOKIRIKA_RD_EH32_H
