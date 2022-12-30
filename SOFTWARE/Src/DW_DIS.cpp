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
void DW_DIS::initial() {
    DW_LCD::Timer_Link(this);
    RTC_H::init();
    DW_LCD::RtC_link(this);
    if(this->TEOM_link!= nullptr) {
        this->SetBackLight((uint8_t) this->TEOM_link->DATA.to_float.dis_light);
        this->set_dis_sleep_time((uint8_t) this->TEOM_link->DATA.to_float.dis_time);
    }
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
 * 触摸屏软件  链接TEOM温控
 * @return
 */
void DW_DIS::Link_Temp(TEOM_TEMP *TEMP_l) {
    this->TEMP_link=TEMP_l;
}
/*!
 * 触摸屏软件  链接通信模块
 * @return
 */
void DW_DIS::Link_Com(Communication *COM_l) {
    this->COM_link=COM_l;
}
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
DW_DIS::Event DW_DIS::key_check() {
    if (this->get_key_sata()) {
        if (this->get_key_address() == key_address)
            return KEY;
        else if ((this->get_key_address() >= Data_address)&&
                (this->get_key_address() < pic_address))
            return Data;
    }
    return Error;
}
/*!
 * 触摸屏软件  获取返回值的地址 排序
 * @return 返回值地址 排序
 */
uint8_t DW_DIS::get_value_address() {
    return (this->get_key_address()-Data_address)/Value_interval;
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
    DW_DIS::Event event = this->key_check();
    if (event==KEY) {
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
            case 15:
                this->Password_page(KEY);
                break;
        }
    }
    else if(event==Data){
        switch (this->get_curInterface()) {
            case 5:
                this->Samp_prepare_page(Data);
                break;
            case 7:
                this->Query_page(Data);
                break;
            case 9:
                this->Maintain_page(Data);
                break;
            case 11:
                this->Settings_page(Data);
                break;
            case 15:
                this->Password_page(Data);
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
                    this->Password_page(TURN);
                    break;
                case 4:
                    this->Settings_page(TURN);
                    break;
            }
            break;
        case DISPLAY:
            break;
        case Data:
            break;
        case Error:
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
                    this->TEOM_link->DATA.to_float.Samp_mode=Samp_Long_mode;
                    break;
                case 4:
                    this->Check_Box_set(Samp_mode_s);
                    this->TEOM_link->DATA.to_float.Samp_mode=Samp_Short_mode;
                    break;
            }
            break;
        case DISPLAY:
            this->vspf_Text(TEXT_ADD(1),(char *)"未知地点");
            this->vspf_Text(TEXT_ADD(2),(char *)"未知人员");
            this->vspf_Text(TEXT_ADD(3),(char *)"%02d-%02d %02d：%02d",RTX->get_month(),RTX->get_day(),RTX->get_hour(),RTX->get_min());
            this->vspf_Text(TEXT_ADD(4),(char *)"%03.1f 小时",TEOM_link->DATA.to_float.Work_TL);
            this->vspf_Text(TEXT_ADD(5),(char *)"%02.0f 小时",TEOM_link->DATA.to_float.Samp_TL);
            this->vspf_Text(TEXT_ADD(6),(char *)"%03.1f 小时",TEOM_link->DATA.to_float.Work_TS);
            this->vspf_Text(TEXT_ADD(7),(char *)"%02.0f 分钟",TEOM_link->DATA.to_float.Samp_TS);
            this->vspf_Text(TEXT_ADD(8),(char *)"%2.0f",TEOM_link->DATA.to_float.Samp_num);
            break;
        case Data:
            float value;
            switch (this->get_value_address()) {
                case 0:
                    value=this->get_value_data();
                    value/=10.0f;
                    if(value>24) value=24;
                    this->TEOM_link->data_save(&TEOM_link->DATA.to_float.Work_TL,abs(value));
                    break;
                case 1:
                    value=this->get_value_data();
                    if(value>24) value=24;
                    this->TEOM_link->data_save(&TEOM_link->DATA.to_float.Samp_TL,abs(value));
                    break;
                case 2:
                    value=this->get_value_data();
                    value/=10.0f;
                    if(value>24) value=24;
                    this->TEOM_link->data_save(&TEOM_link->DATA.to_float.Work_TS,abs(value));
                    break;
                case 3:
                    value=this->get_value_data();
                    if(value>60) value=60;
                    this->TEOM_link->data_save(&TEOM_link->DATA.to_float.Samp_TS,abs(value));
                    break;
            }
            break;
        case Error:
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
        case Data:
            break;
        case Error:
            break;
    }
}
/*!
 * 触摸屏软件  维护页面事件处理
 */
void DW_DIS::Maintain_page(Event E) {
    static float stove_work_flag=1;
    static float flow_work_flag=1;
    switch (E) {
        case TURN:
            this->Interface_switching(9);
            this->clear_text(8);
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0://返回
                    this->Main_page(TURN);
                    break;
                case 1://开关加热炉
                    COM_link->data_set(&COM_link->data_BUS.to_float.stove_work,stove_work_flag);
                    break;
                case 2://开关加热炉
                    COM_link->data_set(&COM_link->data_BUS.to_float.Flow_work,flow_work_flag);
                    break;
                case 3://开关加热炉
                    COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_s,5);
                    break;
                case 4://开关加热炉
                    COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_s,16.7);
                    break;
            }
            break;
        case DISPLAY:
            if(COM_link->data_BUS.to_float.stove_work>0){
                this->vspf_Text(TEXT_ADD(2),(char *)"工作");
                this->Check_Box_set(stove_work_ON, true);
                stove_work_flag=0;
            }
            else{
                this->vspf_Text(TEXT_ADD(2),(char *)"待机");
                this->Check_Box_set(stove_work_ON, false);
                stove_work_flag=1;
            }
            if(COM_link->data_BUS.to_float.Flow_work>0){
                this->Check_Box_set(flow_work_ON, true);
                flow_work_flag=0;
            }
            else{
                this->Check_Box_set(flow_work_ON, false);
                flow_work_flag=1;
            }
            this->vspf_Text(TEXT_ADD(1),(char *)"%05.1lf",COM_link->data_BUS.to_float.stove_temp_r);
            this->vspf_Text(TEXT_ADD(3),(char *)"%05.1lf",COM_link->data_BUS.to_float.Flow_value_r);

            for (int ii = 0; ii < 5; ii++) {
                TEMP_link->CTRLT[ii].upset(false);
                this->vspf_Text(TEXT_ADD(4+ii),(char *)"%05.2lf",TEMP_link->CTRLT[ii].get_cur());
            }
            break;
        case Data:
            break;
        case Error:
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
            this->RTC_Read();
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0://返回
                    this->Main_page(TURN);
                    break;
                case 1:
                    if(this->get_dis_sleep_time()>0) {
                        this->set_dis_sleep_time(this->get_dis_sleep_time() - 1);
                        this->TEOM_link->data_save(&TEOM_link->DATA.to_float.dis_time, this->get_dis_sleep_time());
                    }
                    break;
                case 2:
                    if(this->get_dis_sleep_time()<60) {
                        this->set_dis_sleep_time(this->get_dis_sleep_time() + 1);
                        this->TEOM_link->data_save(&TEOM_link->DATA.to_float.dis_time, this->get_dis_sleep_time());
                    }
                    break;
                case 3:
                    if(this->get_cur_light()>4) {
                        this->SetBackLight(this->get_cur_light() - 4);
                        this->TEOM_link->data_save(&TEOM_link->DATA.to_float.dis_light, this->get_cur_light());
                    }
                    break;
                case 4:
                    if(this->get_cur_light()<61) {
                        this->SetBackLight(this->get_cur_light() + 4);
                        this->TEOM_link->data_save(&TEOM_link->DATA.to_float.dis_light, this->get_cur_light());
                    }
                    break;
            }
            break;
        case DISPLAY:
            this->vspf_Text(TEXT_ADD(1),(char*)"%02d",get_dis_sleep_time());
            this->vspf_Text(TEXT_ADD(2),(char*)"%02d",get_cur_light());
            this->vspf_Text(TEXT_ADD(3),(char*)"%02.0f 小时",TEOM_link->DATA.to_float.l_Samp);
            this->vspf_Text(TEXT_ADD(4),(char*)"%02.0f 分钟",TEOM_link->DATA.to_float.s_Samp);
            break;
        case Data:
            float value;
            switch (this->get_value_address()) {
                case 0:
                    value=this->get_value_data();
                    this->RTX->set_year((uint16_t)value);
                    this->RTC_Write();
                    break;
                case 1:
                    value=this->get_value_data();
                    if(value>12) value=12;
                    this->RTX->set_month((uint8_t)value);
                    this->RTC_Write();
                    break;
                case 2:
                    value=this->get_value_data();
                    if(value>31) value=31;
                    this->RTX->set_day((uint8_t)value);
                    this->RTC_Write();
                    break;
                case 3:
                    value=this->get_value_data();
                    if(value>23) value=23;
                    this->RTX->set_hour((uint8_t)value);
                    this->RTC_Write();
                    break;
                case 4:
                    value=this->get_value_data();
                    if(value>59) value=59;
                    this->RTX->set_min((uint8_t)value);
                    this->RTC_Write();
                    break;
                case 5:
                    value=this->get_value_data();
                    if(value>59) value=59;
                    this->RTX->set_sec((uint8_t)value);
                    this->RTC_Write();
                    break;
                case 6:
                    value=this->get_value_data();
                    if(value>24) value=24;
                    this->TEOM_link->data_save(&TEOM_link->DATA.to_float.l_Samp,abs(value));
                    break;
                case 7:
                    value=this->get_value_data();
                    if(value>60) value=60;
                    this->TEOM_link->data_save(&TEOM_link->DATA.to_float.s_Samp,abs(value));
                    break;
            }
            break;
        case Error:
            break;
    }
}

void DW_DIS::Password_page(DW_DIS::Event E) {
    switch (E) {
        case TURN:
            this->Interface_switching(15);
            this->clear_text(1);
            this->vspf_Text(TEXT_ADD(1),(char*)"请输入密码");
            break;
        case KEY:
            if(this->get_key_data()==0)
                this->Main_page(TURN);
            break;
        case Data:
            if(this->get_value_data()==2008)
                this->Maintain_page(TURN);
            else
                this->vspf_Text(TEXT_ADD(1),(char*)"密码错误！！");
            break;
        case DISPLAY:
            break;
        case Error:
            break;
    }
}











