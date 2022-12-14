/**
* @Author kokirika
* @Name DW_DIS
* @Date 2022-12-13
**/

#ifndef KOKIRIKA_DW_DIS_H
#define KOKIRIKA_DW_DIS_H


#include "USART.h"
#include "DW_LCD.h"

#define TEXT_MAX_NUM  10
#define PIC_MAX_NUM  10

typedef struct k_Check_Box{
    uint16_t address{};
    uint16_t PictureID{};
    uint16_t IDs_x{};
    uint16_t IDs_y{};
    uint16_t IDe_x{};
    uint16_t IDe_y{};
    k_Check_Box(uint16_t add,uint16_t id,uint16_t sx,\
                uint16_t sy,uint16_t ex,uint16_t ey):
            address(add),PictureID(id),IDs_x(sx),\
            IDs_y(sy),IDe_x(ex),IDe_y(ey){}
}Check_Box;


class DW_DIS: private _USART_, public DW_LCD, private Timer{
protected:
    bool key_check();
    enum Event{
        TURN,
        KEY,
        DISPLAY,
    };
private:
    std::function<float()> localfunxx[TEXT_MAX_NUM];
    void CHANGE_PIC(uint16_t address,uint16_t IDs_x,uint16_t IDs_y,uint16_t IDe_x,uint16_t IDe_y);
    void Check_Box_set(Check_Box data,bool en=true);
    void Check_page(Event E);
    void Main_page(Event E);
    void Samp_prepare_page(Event E);
    void Query_page(Event E);
    void Maintain_page(Event E);
    void Settings_page(Event E);

public:
    DW_DIS(USART_TypeDef* USARTx,TIM_TypeDef *TIMx, uint16_t frq);
    void init();
    void key_handle();
    void Dis_handle();
};


#endif //KOKIRIKA_DW_DIS_H
