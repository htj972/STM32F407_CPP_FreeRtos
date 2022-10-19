/**
* @Author kokirika
* @Name RTC_DS32XX
* @Date 2022-10-19
**/

#include "RTC_DS3231.h"

RTC_DS32xx::RTC_DS32xx(Software_IIC *IICx,uint8_t addr) {
    this->IIC=IICx;
    this->Addr=addr;
}

uint8_t RTC_DS32xx::read_data(uint8_t cmd) const {
    return this->IIC->Read_One_Byte(this->Addr,cmd);
}

void RTC_DS32xx::RTC_Read()
{
    uint8_t rtc_current[8]={0x20};
    rtc_current[1]=this->read_data(RTC_DS32xx::Register::rtc_year);
    rtc_current[2]=this->read_data(RTC_DS32xx::Register::rtc_mon);
    rtc_current[3]=this->read_data(RTC_DS32xx::Register::rtc_date);
    rtc_current[4]=this->read_data(RTC_DS32xx::Register::rtc_hour)&0x3f;
    rtc_current[5]=this->read_data(RTC_DS32xx::Register::rtc_min)&0x7f;
    rtc_current[6]=this->read_data(RTC_DS32xx::Register::rtc_sec)&0x7f;
    rtc_current[7]=this->read_data(RTC_DS32xx::Register::rtc_week);

    //-------年-------//
    this->year  =   ((rtc_current[1]&0xf0)>>4)*10+(rtc_current[1]&0x0f);
    //-------月-------//
    this->month =   ((rtc_current[2]&0xf0)>>4)*10+(rtc_current[2]&0x0f);
    //-------日-------//
    this->day   =   ((rtc_current[3]&0xf0)>>4)*10+(rtc_current[3]&0x0f);
    //-------时-------//
    this->hour  =   ((rtc_current[4]&0xf0)>>4)*10+(rtc_current[4]&0x0f);
    //-------分-------//
    this->min   =   ((rtc_current[4]&0xf0)>>4)*10+(rtc_current[4]&0x0f);
    //-------秒-------//
    this->sec   =   ((rtc_current[4]&0xf0)>>4)*10+(rtc_current[4]&0x0f);
    //--------------//
    this->week   = (rtc_current[7]&0x0f);

    if(this->get_week_mode()==RTCBASE::WEEK_MODE::Sunday_First){
        this->week= this->week-1;
        if ( this->week == 0 ) this->week = 7;
        this->name=RTCBASE::Day_Name::Mon+this->week-1;
    }
    this->name=RTCBASE::Day_Name::Mon+this->week-1;
}




