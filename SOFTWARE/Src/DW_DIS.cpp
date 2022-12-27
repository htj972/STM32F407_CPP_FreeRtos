/**
* @Author kokirika
* @Name DW_DIS
* @Date 2022-12-13
**/

#include "DW_DIS.h"
#include "DW_SITE_DATA.h"


/*!
 * 触摸屏软件  构造
 * @继承 串口、迪文屏驱动、定时器
 * @连接 串口到迪文屏驱动
 * @初始化  串口、迪文屏驱动、定时器
 *
 */
DW_DIS::DW_DIS(USART_TypeDef *USARTx, TIM_TypeDef *TIMx, uint16_t frq):frqx(frq) {
    _USART_::init(USARTx,115200);
    DW_LCD::init(this);
    Timer::init(TIMx,10000/frq,8400,true);
}
/*!
 * 触摸屏软件  初始化
 * @连接定时器中断
 * @return
 */
void DW_DIS::init() {
    DW_LCD::Timer_Link(this);
    DW_LCD::RtC_link(this);
    this->Check_page(TURN);
}
/*!
 * 触摸屏软件  链接TEOM
 * @return
 */
void DW_DIS::Link_TEOM(TEOM_Machine *TEOM_l){
    this->TEOM_link=TEOM_l;
};
/*!
 * 触摸屏软件  设置屏保时间 分钟
 * @连接定时器中断
 * @return
 */
void DW_DIS::set_dis_sleep_time(uint16_t x) {
    DW_LCD::set_dis_sleep_time(frqx*60*x);
}
/*!
 * 触摸屏软件  获取屏保时间 分钟
 * @连接定时器中断
 * @return
 */
uint16_t DW_DIS::get_dis_sleep_time() const {
    return DW_LCD::get_dis_sleep_time()/frqx/60;
}
/*!
 * 触摸屏软件  按键准备检测
 * @return 按键处理准备完毕
 */
bool DW_DIS::key_check() {
    if (this->get_key_sata())
        if(this->get_key_address()==key_address)
            return true;
    return false;
}
/*!
 * 触摸屏软件  文本清除
 * @return
 */
bool DW_DIS::clear_text(uint8_t num) {
    char void_char_str[]="                      ";
    for (int ii = 0; ii <= num ; ii++) {
        this->vspf_Text(TEXT_ADD(ii),void_char_str);
    }
    return true;
}
/*!
 * 触摸屏软件  基本图形同位置切割
 * @param address 基本图形位置
 * @param IDs_x IDs_y 开始位置
 * @param IDe_x IDe_y 结束位置
 * @return
 */
void DW_DIS::CHANGE_PIC(uint16_t address, uint16_t IDs_x, uint16_t IDs_y, uint16_t IDe_x, uint16_t IDe_y) {
    this->Picture_cut(address,this->get_curInterface()+1,IDs_x,IDs_y,IDe_x,IDe_y,IDs_x,IDs_y);
}
/*!
 * 触摸屏软件  选择单选框
 * @param data 基本图形位置
 * @param en   正反选
 * @return
 */
void DW_DIS::Check_Box_set(Check_Box data, bool en) {
    this->Picture_cut(data.address,en?data.PictureID:this->get_curInterface(),\
    data.IDs_x,data.IDs_y,data.IDe_x,data.IDe_y,data.IDs_x,data.IDs_y);
}
/*!
 * 触摸屏软件  事件处理接口
 * @转送    根据页面id号 转送到当前页面所有按键接口方法
 */
void DW_DIS::key_handle() {
    if (this->key_check()) {
        if(this->get_key_data()==0xFFFF)
            this->Wake_up();
        else
        switch (this->get_curInterface()) {
            case 0:
                this->Wake_up();
                break;
            case 3:
                this->Main_page(KEY);
                break;
            case 5:
                this->Samp_prepare_page(KEY);
                break;
            case 7:
                this->Query_page(KEY);
                break;
            case 9:
                this->Maintain_page(KEY);
                break;
            case 11:
                this->Settings_page(KEY);
                break;
        }
    }
}
/*!
 * 触摸屏软件  显示事件处理接口
 * @转送    根据页面id号 转送到当前页面所有按键接口方法
 */
void DW_DIS::Dis_handle() {
    switch (this->get_curInterface()) {
        case 1:
            Check_page(DISPLAY);
        case 3:
            this->Main_page(DISPLAY);
            break;
        case 5:
            this->Samp_prepare_page(DISPLAY);
            break;
        case 7:
            this->Query_page(DISPLAY);
            break;
        case 9:
            this->Maintain_page(DISPLAY);
            break;
        case 11:
            this->Settings_page(DISPLAY);
            break;
    }
}
/*!
 * 触摸屏软件  检测数据事件显示处理
 * @根据按键转换对对应页面
 */
void DW_DIS::Check_page(Event E) {
    switch (E) {
        case TURN:
        this->Interface_switching(1);
        this->vspf_Text(TEXT_ADD(1),(char*)"V 0.0");
        this->vspf_Text(TEXT_ADD(2),(char*)"V 0.0");
        this->vspf_Text(TEXT_ADD(3),(char*)"ABCD");
        this->vspf_Text(TEXT_ADD(4),(char*)"OK");
    break;
    case DISPLAY:
        static uint8_t ii=0;
            this->set_Progress_bar(PIC_ADD(1),ii,87,362,724,399);
            ii+=10;
            if(ii>=100)
                Main_page(TURN);
        break;
        case KEY:
            break;
    }
}
/*!
 * 触摸屏软件  主页面事件处理
 * @根据按键转换对对应页面
 */
void DW_DIS::Main_page(Event E) {
    switch (E) {
        case TURN:this->Interface_switching(3);

            break;
        case KEY:
            switch (this->get_key_data()) {
                case 1:
                    this->Samp_prepare_page(TURN);
                    break;
                case 2:
                    this->Query_page(TURN);
                    break;
                case 3:
                    this->Maintain_page(TURN);
                    break;
                case 4:
                    this->Settings_page(TURN);
                    break;
            }
            break;
        case DISPLAY:
            break;
    }
}
/*!
 * 触摸屏软件  采样页面事件处理
 */
void DW_DIS::Samp_prepare_page(Event E) {
    switch (E) {
        case TURN:this->Interface_switching(5);
            this->Check_Box_set(Samp_mode_l);
            this->clear_text(8);
            this->RTC_Read();
            TEOM_link->DATA.to_float.Samp_TL=TEOM_link->DATA.to_float.l_Samp;
            TEOM_link->DATA.to_float.Samp_TS=TEOM_link->DATA.to_float.s_Samp;
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0://返回
                    this->Main_page(TURN);
                    break;
                case 3:
                    this->Check_Box_set(Samp_mode_l);
                    break;
                case 6:
                    this->Check_Box_set(Samp_mode_s);
                    break;
            }
            break;
        case DISPLAY:
            this->vspf_Text(TEXT_ADD(1),(char *)"未知地点");
            this->vspf_Text(TEXT_ADD(2),(char *)"未知人员");
            this->vspf_Text(TEXT_ADD(3),(char *)"%02d-%02d %02d：%02d",RTX->get_month(),RTX->get_day(),RTX->get_hour(),RTX->get_min());
            this->vspf_Text(TEXT_ADD(4),(char *)"%02.0f 小时",TEOM_link->DATA.to_float.Work_TL);
            this->vspf_Text(TEXT_ADD(5),(char *)"%02.0f 小时",TEOM_link->DATA.to_float.Samp_TL);
            this->vspf_Text(TEXT_ADD(6),(char *)"%02.0f 分钟",TEOM_link->DATA.to_float.Work_TS);
            this->vspf_Text(TEXT_ADD(7),(char *)"%02.0f 分钟",TEOM_link->DATA.to_float.Samp_TS);
            this->vspf_Text(TEXT_ADD(8),(char *)"%2.0f",TEOM_link->DATA.to_float.Samp_num);
            break;
    }

}
/*!
 * 触摸屏软件  查询页面事件处理
 */
void DW_DIS::Query_page(Event E) {
    switch (E) {
        case TURN:this->Interface_switching(7);
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0://返回
                    this->Main_page(TURN);
                    break;
            }
            break;
        case DISPLAY:
            break;
    }
}
/*!
 * 触摸屏软件  维护页面事件处理
 */
void DW_DIS::Maintain_page(Event E) {
    switch (E) {
        case TURN:
            this->Interface_switching(9);
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0://返回
                    this->Main_page(TURN);
                    break;
            }
            break;
        case DISPLAY:
            break;
    }
}
/*!
 * 触摸屏软件  设置页面事件处理
 */
void DW_DIS::Settings_page(Event E) {
    switch (E) {
        case TURN:
            this->Interface_switching(11);
            this->clear_text(4);
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0://返回
                    this->Main_page(TURN);
                    break;
                case 1:
                    if(this->get_dis_sleep_time()>0)
                        this->set_dis_sleep_time(this->get_dis_sleep_time()-1);
                    break;
                case 2:
                    if(this->get_dis_sleep_time()<60)
                        this->set_dis_sleep_time(this->get_dis_sleep_time()+1);
                    break;
                case 3:
                    if(this->get_cur_light()>4)
                        this->SetBackLight(this->get_cur_light()-4);
                    break;
                case 4:
                    if(this->get_cur_light()<61)
                        this->SetBackLight(this->get_cur_light()+4);
                    break;
                case 5:
                    this->RTC_Read();
                    break;
            }
            break;
        case DISPLAY:
            this->vspf_Text(TEXT_ADD(1),(char*)"%02d",get_dis_sleep_time());
            this->vspf_Text(TEXT_ADD(2),(char*)"%03d",get_cur_light());
            this->vspf_Text(TEXT_ADD(3),(char*)"%02.0f 小时",TEOM_link->DATA.to_float.l_Samp);
            this->vspf_Text(TEXT_ADD(4),(char*)"%02.0f 分钟",TEOM_link->DATA.to_float.s_Samp);
            break;
    }
}









