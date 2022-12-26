/**
* @Author kokirika
* @Name FOWER_CTRL
* @Date 2022-12-26
**/

#include <valarray>
#include "Fower_Ctrl.h"

float Fower_Ctrl::calculation_hole_flow() {
    float fbuf = this->liuya->get_pres();
    if(fbuf < 2)        fbuf = 0;  //如果动压小于2Pa，忽略，防止漂移
    else
    {
        //fbuf = fbuf * (273.15 + JQ_Tr.value)/(BAR.value + JY.value) * 0.28689097;
        fbuf = fbuf / 1.34f * (273.15f + jiwen->get_temp()) / 273.15f * 101.325f / (daqiya->get_pres() + jiya->get_pres());
    }

    if(fbuf<0)  fbuf=0; //结果小于0，忽略
    else
    {
        fbuf = (float )sqrt((double )fbuf) * FLOW_RATE;  //计算孔板处流量
    }
    return fbuf;
}

float Fower_Ctrl::calculation_entrance_flow() {
    //折算到入口流量
    return this->calculation_hole_flow() * (daqiya->get_pres() + jiya->get_pres()) *
            (273.15f + daqiwen->get_temp()) / daqiya->get_pres() / (273.15f + jiwen->get_temp());
}
