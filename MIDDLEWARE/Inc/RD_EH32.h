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
    void Reset();                               //��λ
    void Next_Page();                           //��ҳ
    void Enter();                               //��ӡ����
    void Move(float mm=0.5);                    //��ֽ����
    void Moveline(uint8_t line=1);              //��ֽ����
    void Reverse(bool sata=true);               //��ת��ӡ
    void Hori_table(int n,...);                 //ˮƽ�Ʊ�
    void Hori_Next_table();                     //�ƶ��Ʊ�
    void set_Overline(bool IS=true);            //�ϻ���
    void set_underline(bool IS=true);           //�»���
    void Reverse_color(bool IS=true);           //��ɫ��ת
    void set_Rotation(ANGLE angle=ANGLE::_0);   //�ַ���ת
    void set_Site(uint16_t site);               //���þ���λ��
    void set_notprint_Lweight(uint8_t Byte);    //����ֹ��ӡ
    void set_notprint_Rweight(uint8_t Byte);    //�Ҳ��ֹ��ӡ
    void set_Row_spacing(uint8_t point);        //�����м��
    void set_Word_spacing(uint8_t point);       //�����ּ��
    void set_Alignment(ALIGN mode);             //���ö���
    void set_Vertical_magnification(uint8_t Mult);//ˮƽ�Ŵ�1~8
    void set_Horizontal_magnification(uint8_t Mult);//��ֱ�Ŵ�1~8
    void set_magnification(uint8_t Multx,uint8_t Multy);//�ַ��Ŵ�1~8 ˮƽ ��ֱ

    void Drow_pic1();                             //��ӡͼƬ ���ԷŴ�
    void Drow_pic2();                             //��ӡͼƬ  �����ԷŴ�
    void Drow_Raster_bitmap();                    //��ӡ��դλͼ
};




#endif //KOKIRIKA_RD_EH32_H
