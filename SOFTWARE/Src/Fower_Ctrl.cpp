/**
* @Author kokirika
* @Name FOWER_CTRL
* @Date 2022-12-26
**/


#include <cmath>
#include "Fower_Ctrl.h"
/*!
 * 流量控制构造
 */
Fower_Ctrl::Fower_Ctrl(Pressure_BASE *liu, Pressure_BASE *ji, Pressure_BASE *daqi,
                       Temperature_BASE *jit,Temperature_BASE *daqit,GPIO_Pin FA): PID_BASE(),
                       liuya(liu),jiya(ji),daqiya(daqi),jiwen(jit),daqiwen(daqit){
    this->filter[0].init(15,5);//*liuya;
    this->filter[1].init(15,5);//*jiya;
    this->filter[2].init(15,5);//*daqiya;
    this->filter[3].init(5,1);//*jiwen;
    this->filter[4].init(5,1);//*daqiwen;
    this->filter[5].init(5,2);//*FF_value;
    this->FAx.init(FA,HIGH);
    for(auto & ii : this->data_t){
        for (float & jj : ii)
            jj=0;
    }
    for (uint8_t & ii : this->data_n)
        ii=0;

    PID_BASE::init(0.8,0.4,1);
}
/*!
 * 折算孔板流量
 */
float Fower_Ctrl::calculation_hole_flow() {
    float fbuf = LiuYa;
    float density_1=0.28689097f*(273.15f+JiWen)/(DaQiYa*1000+JiYa);

    if(density_1<0) {FF_value=0;return 0;} //结果小于0，忽略
    if(fbuf < 2)        fbuf = 0;  //如果动压小于2Pa，忽略，防止漂移
    fbuf = (float )sqrt((double )(fbuf*density_1)) * FLOW_RATE;  //计算孔板处流量

    this->data_t[5][this->data_n[5]++]=fbuf;
    if (this->data_n[5] >= 10)this->data_n[5] = 0;
    float sum=0;
    for(uint8_t ii=0; ii<10;ii++) {
        sum+=this->data_t[5][ii];
    }
    fbuf=sum/10.0f;
    FF_value=fbuf;
//    cur=FF_value;
    return fbuf;
}
/*!
 * 折算到入口流量
 */
float Fower_Ctrl::calculation_entrance_flow() {
    float curL=this->calculation_hole_flow() * (DaQiYa*1000 +JiYa) *
        (273.15f + DaQiWen) / (DaQiYa*1000) / (273.15f + JiWen);

//    if(curL>0) {
//        this->data_t[5][this->data_n[5]++] = this->filter[5].Filter(cur);
//        if (this->data_n[5] >= 10)this->data_n[5] = 0;
//        float sum = 0;
//        for (uint8_t ii = 0; ii < 10; ii++) {
//            sum += this->data_t[5][ii];
//        }
//        FF_value = sum / 10.0f;
//    }
//    else{
//        FF_value = 0;
//    }
//    FF_value = curL;
//    return FF_value;
    return curL;
//    return this->calculation_hole_flow() * (DaQiYa*1000 +JiYa) *
//            (273.15f + DaQiWen) / (DaQiYa*1000) / (273.15f + JiWen);
}
/*!
 * 计算倍率
 * @param value 实际入口流量
 */
void Fower_Ctrl::FLOW_RATE_change(float value) {
//    cur*(DaQiYa*1000)*(273.15f + JiWen)/ (DaQiYa*1000 +JiYa) /(273.15f + DaQiWen)
//    FLOW_RATE=value/FF_value*FLOW_RATE;
    FLOW_RATE=value*(DaQiYa*1000)*(273.15f + JiWen)/ (DaQiYa*1000 +JiYa)
            /(273.15f + DaQiWen)/FF_value*FLOW_RATE;
}
/*!
 * 同步数据
 */
void Fower_Ctrl::data_upset() {
    float data_g[5];
    data_g[0]=liuya->get_pres();
    data_g[1]=jiya->get_pres();
    data_g[2]=daqiya->get_pres();
    data_g[3]=jiwen->get_temp();
    data_g[4]=daqiwen->get_temp();

    for(uint8_t ii=0;ii<5;ii++){
//        if(data_g[ii]>0)
            this->data_t[ii][this->data_n[ii]++]=filter[ii].Filter(data_g[ii]);
    }

    for(uint8_t & ii : this->data_n) {
        if (ii >= Fower_Data_num)ii = 0;
    }

    float sum=0;
    for(float & ii : this->data_t[0]) {
        sum+=ii;
    }
    LiuYa=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[1]) {
        sum+=ii;
    }
    JiYa=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[2]) {
        sum+=ii;
    }
    DaQiYa=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[3]) {
        sum+=ii;
    }
    JiWen=sum/=Fower_Data_num;

    sum=0;
    for(float & ii : this->data_t[4]) {
        sum+=ii;
    }
    DaQiWen=sum/=Fower_Data_num;
}
/*!
 * 同步流压数据
 */
void Fower_Ctrl::LiuYa_data_upset() {
    float data_g=liuya->get_pres();
    this->data_t[0][this->data_n[0]++]=filter[0].Filter(data_g);
    if (this->data_n[0] >= Fower_Data_num)this->data_n[0] = 0;
    float sum=0;
    for(float & ii : this->data_t[0]) {
        sum+=ii;
    }
    LiuYa=sum/=Fower_Data_num;
}
/*!
 * 同步计压数据
 */
void Fower_Ctrl::JiYa_data_upset() {
    float data_g=jiya->get_pres();
    this->data_t[1][this->data_n[1]++]=filter[1].Filter(data_g);
    if (this->data_n[1] >= Fower_Data_num)this->data_n[1] = 0;
    float sum=0;
    for(float & ii : this->data_t[1]) {
        sum+=ii;
    }
    JiYa=sum/=Fower_Data_num;
}
/*!
 * 同步大气压数据
 */
void Fower_Ctrl::DaQiYa_data_upset() {
    float data_g=daqiya->get_pres();
    this->data_t[2][this->data_n[2]++]=filter[2].Filter(data_g);
    if (this->data_n[2] >= Fower_Data_num)this->data_n[2] = 0;
    float sum=0;
    for(float & ii : this->data_t[2]) {
        sum+=ii;
    }
    DaQiYa=sum/=Fower_Data_num;
}
/*!
 * 同步计温数据
 */
void Fower_Ctrl::JiWen_data_upset() {
    float data_g=jiwen->get_temp();
    this->data_t[3][this->data_n[3]++]=filter[3].Filter(data_g);
    if (this->data_n[3] >= Fower_Data_num)this->data_n[3] = 0;
    float sum=0;
    for(float & ii : this->data_t[3]) {
        sum+=ii;
    }
    JiWen=sum/=Fower_Data_num;
}
/*!
 * 同步大气温度数据
 */
void Fower_Ctrl::DaQiWen_data_upset() {
    float data_g=daqiwen->get_temp();
    this->data_t[4][this->data_n[4]++]=filter[4].Filter(data_g);
    if (this->data_n[4] >= Fower_Data_num)this->data_n[4] = 0;
    float sum=0;
    for(float & ii : this->data_t[4]) {
        sum+=ii;
    }
    DaQiWen=sum/=Fower_Data_num;
}
/*!
 * 更新PID控制输出
 */
void Fower_Ctrl::upset() {
    float OUT;
    cur=FF_value;

    if(en){
        OUT=this->calculate(cur);
        if(OUT>100)OUT=100;
        else if(OUT<4)OUT=4;

        if(this->CONTROLLER!= nullptr)
            this->CONTROLLER->set_Duty_cycle(this->chx,OUT);
    } else{
        if(this->CONTROLLER!= nullptr) {
            OUT=this->calculate(0,cur);
            if(OUT>100)OUT=100;
            else if(OUT<0)OUT=0;
            this->CONTROLLER->set_Duty_cycle(this->chx,OUT);
        }
        cur=0;
        this->clear();
    }
}
/*!
 * 配置PWM类
 * @param CONTR PWM类指针
 * @param ch  PWM通道
 */
void Fower_Ctrl::config(PWM_H *CONTR, uint8_t ch) {
    CONTROLLER=CONTR,
    chx=ch;
}
/*!
 * 开启输出
 */
void Fower_Ctrl::TURN_ON() {
    if(!en)
        en= true;
    this->FAx.set(ON);
}
/*!
 * 关闭输出
 */
void Fower_Ctrl::TURN_OFF() {
    if(en)
        en= false;
    this->FAx.set(OFF);
}



