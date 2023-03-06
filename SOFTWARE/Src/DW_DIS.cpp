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
 * 触摸屏软件  曲线数据
 * @return
 */
int      DW_DIS::teom_dis_mul_num=1;		//放大角标
uint16_t DW_DIS::teom_dis_mul[3][2]={{935,5},{187,25},{37,125}}; //放大系数
void DW_DIS::send_cure_data(uint8_t ch,float center,float data) {
    static uint8_t mark=0;
    if(mark!= this->RTX->get_sec()) {
        mark= this->RTX->get_sec();
        if (data < center-1)
            this->DROW_point(ch, 0);
        else if ((data > center-1) && (data < center+1)) {
            this->DROW_point(ch, (uint16_t) ((data - center) * 10000));
            this->Write_data(point_address, 0x06, (uint16_t) ((data - center) * 10000));
        } else if (data > center+1)
            this->DROW_point(ch, 10000);
    }

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
            case 16:
                this->Super_page(KEY);
                break;
            case 17:
            case 18:
                this->Working_page(KEY);
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
            case 16:
                this->Super_page(Data);
                break;
            case 17:
                this->Working_page(Data);
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
        case 16:
            this->Super_page(DISPLAY);
            break;
        case 17:
        case 18:
            this->Working_page(DISPLAY);
            break;
    }
}
/*!
 * 触摸屏软件  检测数据事件
 * @根据按键转换对对应页面
 */
#include "delay.h"
#define Event_num 10
uint8_t DW_DIS::Check_Event(uint8_t num){
    switch (num/(100/Event_num)){
        case 0:
            if(COM_link->modbus_03_send(0,4)==0)
                this->vspf_Text(TEXT_ADD(4),(char*)"通讯正常");
            else
                this->vspf_Text(TEXT_ADD(4),(char*)"通讯异常");
            break;
        default:
            this->vspf_Text(TEXT_ADD(4),(char*)"  %d      ",num/(100/Event_num));
            break;
    }
    delay_ms(500);
    num+=(100/Event_num);
    return num;
}
/*!
 * 触摸屏软件  检测数据事件显示处理
 * @根据按键转换对对应页面
 */
void DW_DIS::Check_page(Event E) {
    switch (E) {
        case TURN:
        this->Interface_switching(1);
        this->vspf_Text(TEXT_ADD(1),(char*)"V3.1f",TEOM_link->DATA.to_float.version);
        this->vspf_Text(TEXT_ADD(2),(char*)"V1.0");
        this->vspf_Text(TEXT_ADD(3),(char*)"ABCD");
    break;
    case DISPLAY:
        static uint8_t ii=0;
            ii=Check_Event(ii);
            this->set_Progress_bar(PIC_ADD(1),ii,87,362,724,399);
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
                case 6:
                    this->Working_page(TURN);
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
                case 2://标定开关
                    COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_s,5);
                    COM_link->data_set(&COM_link->data_BUS.to_float.Flow_work,flow_work_flag);
                    break;
                case 3://5L
                    COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_s,5);
                    break;
                case 4://17L
                    COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_s,15.5);
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
            this->vspf_Text(TEXT_ADD(1),(char *)"%05.1lf"
                            ,COM_link->data_BUS.to_float.stove_temp_r);
            this->vspf_Text(TEXT_ADD(3),(char *)"%05.1lf   %05.2lf",COM_link->data_BUS.to_float.Flow_value_r,
                            COM_link->data_BUS.to_float.Flow_coefficient);

            for (int ii = 0; ii < 5; ii++) {
                TEMP_link->CTRLT[ii].upset(false);
                this->vspf_Text(TEXT_ADD(4+ii),(char *)"%05.2lf",TEMP_link->CTRLT[ii].get_cur());
            }
            break;
        case Data:
            COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_w,this->get_value_data()/100.0f);
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
    static uint8_t error_f=0;
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
            if(this->get_value_data()==2007)
                this->Maintain_page(TURN);
            else if(this->get_value_data()==87) {
                error_f = 1;
                this->vspf_Text(TEXT_ADD(1), (char *) "密码错误！！");
            }
            else if((error_f==1)&&(this->get_value_data()==2012)) {
                error_f = 0;
                this->Super_page(TURN);
            }
            else {
                error_f = 0;
                this->vspf_Text(TEXT_ADD(1), (char *) "密码错误！！");
            }
            break;
        case DISPLAY:
            break;
        case Error:
            break;
    }
}

void DW_DIS::Working_page(DW_DIS::Event E) {
    float  frq_temp,frq_center;
    switch (E) {
        case TURN:
            this->Interface_switching(17);
            this->clear_text(8);
            this->vspf_Text(TEXT_ADD(3), (char *) "00:00:00");
            this->TEOM_link->turn_on();
            this->Write_data(point_address,0x08,teom_dis_mul[teom_dis_mul_num][0]);
            this->vspf_Text(TEXT_ADD(9),(char *)"0.0%02dhz/行 ",teom_dis_mul[teom_dis_mul_num][1]/5);
            worked = false;
            if(this->TEOM_link->DATA.to_float.Samp_mode==Samp_Long_mode) {
                COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_s,5);
                COM_link->data_set(&COM_link->data_BUS.to_float.Flow_work,1);
            }
            else{
                COM_link->data_set(&COM_link->data_BUS.to_float.Flow_value_s,15.5);
                COM_link->data_set(&COM_link->data_BUS.to_float.Flow_work,1);
            }
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0://返回
                    this->Interface_switching(17);
                    break;
                case 1:
                    this->RTX->set_Initial_time();
                    frq_center=this->TEOM_link->get_frq();
                    this->vspf_Text(TEXT_ADD(5),(char *) "%09.5lfHz   ",frq_center);
                    this->vspf_Text(TEXT_ADD(7),(char *) "%04.0lfPa   ",this->TEOM_link->Pre_link->get_value());
                    worked = true;
                break;
                case 2:
                    this->Interface_switching(18);
                    break;
                case 3:
                    teom_dis_mul_num--;
                    if(teom_dis_mul_num<=0)teom_dis_mul_num=0;
                    this->Write_data(point_address,0x08,teom_dis_mul[teom_dis_mul_num][0]);
                    this->vspf_Text(TEXT_ADD(9),(char *)"0.0%02dhz/行 ",teom_dis_mul[teom_dis_mul_num][1]/5);
                    break;
                case 4:
                    teom_dis_mul_num++;
                    if(teom_dis_mul_num>=2)teom_dis_mul_num=2;
                    this->Write_data(point_address,0x08,teom_dis_mul[teom_dis_mul_num][0]);
                    this->vspf_Text(TEXT_ADD(9),(char *)"0.0%02dhz/行 ",teom_dis_mul[teom_dis_mul_num][1]/5);
                    break;
            }
            break;
        case Data:
            if(this->get_key_data()==0xF1) {
                worked = false;
                this->Samp_prepare_page(TURN);
                this->TEOM_link->turn_off();
                COM_link->data_set(&COM_link->data_BUS.to_float.Flow_work,0);
            }
            break;
        case DISPLAY:
            this->vspf_Text(TEXT_ADD(2), (char *) "%05.1lfL/m",COM_link->data_BUS.to_float.Flow_value_r);
            if(this->TEOM_link->DATA.to_float.Samp_mode==Samp_Long_mode) {
                this->vspf_Text(TEXT_ADD(1), (char *) "长时间采样");
                this->vspf_Text(TEXT_ADD(4), (char *) "%04.1lf小时",TEOM_link->DATA.to_float.Samp_TL);
            }
            else{
                this->vspf_Text(TEXT_ADD(1), (char *) "短时间采样");
                this->vspf_Text(TEXT_ADD(4), (char *) "%04.1lf分钟",TEOM_link->DATA.to_float.Samp_TS);
            }
            if(worked){
                uint8_t h,m,s;
                this->RTX->get_run_time(&h,&m,&s);
                this->vspf_Text(TEXT_ADD(3),(char *) "%02d:%02d:%02d  ",h,m,s);
                if(this->TEOM_link->DATA.to_float.Samp_mode==Samp_Long_mode) { //采样完成
                    if(h==(uint8_t)TEOM_link->DATA.to_float.Samp_TL){
                        worked = false;
                        this->Keyboard_Up(0x11);
                        this->TEOM_link->turn_off();
                        COM_link->data_set(&COM_link->data_BUS.to_float.Flow_work,0);
                        TEOM_link->DATA.to_float.Samp_num++;
                        this->TEOM_link->data_save(&TEOM_link->DATA.to_float.dis_light, this->get_cur_light());
                    }
                }
                else{
                    if(m==(uint8_t)TEOM_link->DATA.to_float.Samp_TS){
                        worked = false;
                        this->Keyboard_Up(0x11);
                        this->TEOM_link->turn_off();
                        COM_link->data_set(&COM_link->data_BUS.to_float.Flow_work,0);
                        TEOM_link->DATA.to_float.Samp_num++;
                        this->TEOM_link->data_save(&TEOM_link->DATA.to_float.dis_light, this->get_cur_light());
                    }
                }
                this->send_cure_data(0,frq_center,frq_temp);
            }
            else{
                this->vspf_Text(TEXT_ADD(3),(char *) "00：00：00");
                this->vspf_Text(TEXT_ADD(5),(char *) "---.---Hz");
                this->vspf_Text(TEXT_ADD(7),(char *) "----Pa");
            }
            frq_temp =this->TEOM_link->get_frq();
            this->vspf_Text(TEXT_ADD(6),(char *) "%09.5lfHz   ",frq_temp);

            this->vspf_Text(TEXT_ADD(8),(char *) "%04.0lfPa   ",this->TEOM_link->Pre_link->get_value());

            this->vspf_Text(TEXT_ADD(10), (char *) "%09.5lf",
                            frq_temp + (float) teom_dis_mul[teom_dis_mul_num][1] / 1000.0f);//388.46844
            this->vspf_Text(TEXT_ADD(11), (char *) "%09.5lf", frq_temp);//388.443451
            this->vspf_Text(TEXT_ADD(12), (char *) "%09.5lf",
                            frq_temp - (float) teom_dis_mul[teom_dis_mul_num][1] / 1000.0f);
            break;
        case Error:
            break;
    }
}

void DW_DIS::Super_page(DW_DIS::Event E) {
    switch (E) {
        case TURN:
            this->Interface_switching(16);
            break;
        case KEY:
            switch (this->get_key_data()) {
                case 0:
                this->Main_page(TURN);
            }
            break;
        case Data:
            break;
        case DISPLAY:
            break;
        case Error:
            break;
    }
}












