/**
* @Author kokirika
* @Name DW_DIS
* @Date 2022-12-13
**/

#ifndef KOKIRIKA_DW_DIS_H
#define KOKIRIKA_DW_DIS_H

#include "USART.h"
#include "DW_LCD.h"
#include "TEOM.h"
#include "RTC_H.h"
#include "Communication.h"
#include "Storage_Link.h"

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


class DW_DIS: private _USART_, public DW_LCD, private Timer,private RTC_H{
protected:
    enum Event{
        TURN,
        KEY,
        Data,
        DISPLAY,
        Error,
    };
    uint16_t frqx;
    Event key_check();
    uint8_t Check_Event(uint8_t num);
    uint8_t get_value_address();
    bool clear_text(uint8_t num);
    TEOM_Machine *TEOM_link{};
    TEOM_TEMP    *TEMP_link{};
    Communication *COM_link{};
    Storage_Link  *Flash{};

private:
    bool worked{};
    static int      teom_dis_mul_num;		//放大角标
    static uint16_t teom_dis_mul[3][2];     //放大系数
    float  frq_temp{},frq_center{},press_temp{},press_center{},teom_qua{},Concentration{};
    bool TEMP_UP_F[5]{};
    bool send_cure_data(uint8_t ch,float center,float data);
    void CHANGE_PIC(uint16_t address,uint16_t IDs_x,uint16_t IDs_y,uint16_t IDe_x,uint16_t IDe_y);
    void Check_Box_set(Check_Box data,bool en=true);
    void Check_page(Event E);   //系统检查
    void Main_page(Event E);    //主界面
    void Samp_prepare_page(Event E);    //采样准备界面

    void Query_page(Event E);
    void Maintain_page(Event E);    //维护页面
    void Settings_page(Event E);    //设置页面
    void Password_page(Event E);    //密码页面
    void Working_page(Event E);     //采样进行界面 曲线图续接
    void Super_page(Event E);       //超级维护页面
    void Data_DIS(Event E);         //采样数据页面
    void statement(Event E);          //数据报表


public:
    DW_DIS(USART_TypeDef* USARTx,TIM_TypeDef *TIMx, uint16_t frq);
    void initial();
    void Link_TEOM(TEOM_Machine *TEOM_l);
    void Link_Temp(TEOM_TEMP *TEMP_l);
    void Link_Com(Communication *COM_l);
    void Link_Flash(Storage_Link *flash);
    void key_handle();
    void Dis_handle();
    void set_dis_sleep_time(uint16_t x) override;
    uint16_t get_dis_sleep_time() const override;
};


#endif //KOKIRIKA_DW_DIS_H
