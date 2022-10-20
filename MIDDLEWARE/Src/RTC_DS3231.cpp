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

void RTC_DS32xx::init(Software_IIC *IICx, uint8_t addr) {
    this->IIC=IICx;
    this->Addr=addr;
}

uint8_t RTC_DS32xx::read_data(uint8_t cmd) const {
    if(this->IIC!= nullptr) {
        uint8_t value = this->IIC->Read_One_Byte(this->Addr, cmd);
        return ((value & 0xf0) >> 4) * 10 + (value & 0x0f);
    }
    else
        return 0;
}

void RTC_DS32xx::write_data(uint8_t cmd,uint8_t data) const {
    if(this->IIC!= nullptr) {
        uint8_t value = ((data / 10) << 4) + (data % 10);
        this->IIC->Write_One_Byte(this->Addr, cmd, value);
    }
}

void RTC_DS32xx::set_week() {
    uint8_t w=RTCBASE::week_change();
    w%=7;
    this->write_data(RTC_DS32xx::Register::rtc_week,w);
}

void RTC_DS32xx::set_year(uint16_t y) {
    RTCBASE::set_year(y);
    y%=100;
    this->write_data(RTC_DS32xx::Register::rtc_year,y);
    this->set_week();
}

void RTC_DS32xx::set_month(uint8_t m) {
    RTCBASE::set_month(m);
    m%=12;
    this->write_data(RTC_DS32xx::Register::rtc_mon,m);
    this->set_week();
}

void RTC_DS32xx::set_day(uint8_t d) {
    RTCBASE::set_day(d);
    d%=31;
    this->write_data(RTC_DS32xx::Register::rtc_date,d);
    this->set_week();
}

void RTC_DS32xx::set_hour(uint8_t h, uint8_t ap) {
    RTCBASE::set_hour(h, ap);
    this->set_hour_mode(RTCBASE::HOUR_MODE::_12H_MODE);
    h%=12;
    if(ap==RTCBASE::DAY_APM::PM)
        h+=12;
    this->write_data(RTC_DS32xx::Register::rtc_date,h);
}

void RTC_DS32xx::set_hour(uint8_t h) {
    RTCBASE::set_hour(h);
    h%=24;
    this->write_data(RTC_DS32xx::Register::rtc_hour,h);
}

void RTC_DS32xx::set_apm(uint8_t ap) {
    RTCBASE::set_apm(ap);
    if((this->get_apm()==RTCBASE::DAY_APM::PM)&&(ap==RTCBASE::DAY_APM::AM))
    this->set_hour(this->get_hour(),ap);
}

void RTC_DS32xx::set_min(uint8_t m) {
    RTCBASE::set_min(m);
    m%=60;
    this->write_data(RTC_DS32xx::Register::rtc_min,m);
}

void RTC_DS32xx::set_sec(uint8_t s) {
    RTCBASE::set_sec(s);
    s%=60;
    this->write_data(RTC_DS32xx::Register::rtc_sec,s);
}

void RTC_DS32xx::set_date(uint16_t y, uint8_t m, uint8_t d) {
    RTCBASE::set_date(y, m, d);
    y%=100;
    m%=12;
    d%=31;
    this->write_data(RTC_DS32xx::Register::rtc_year,y);
    this->write_data(RTC_DS32xx::Register::rtc_mon,m);
    this->write_data(RTC_DS32xx::Register::rtc_date,d);
    this->set_week();
}

void RTC_DS32xx::set_time(uint8_t h, uint8_t m, uint8_t s, uint8_t ap) {
    this->set_hour_mode(RTCBASE::HOUR_MODE::_12H_MODE);
    this->set_hour(h,ap);
    this->set_min(m);
    this->set_sec(s);
}

void RTC_DS32xx::set_time(uint8_t h, uint8_t m, uint8_t s) {
    this->set_hour_mode(RTCBASE::HOUR_MODE::_24H_MODE);
    this->set_hour(h);
    this->set_min(m);
    this->set_sec(s);
}

uint8_t RTC_DS32xx::get_week() {
    return this->read_data(RTC_DS32xx::Register::rtc_week);
}

uint16_t RTC_DS32xx::get_year() {
    uint16_t value=2000+this->read_data(RTC_DS32xx::Register::rtc_year);
    RTCBASE::set_year(value);
    return value;
}

uint8_t RTC_DS32xx::get_month() {
    uint8_t value=this->read_data(RTC_DS32xx::Register::rtc_mon);
    RTCBASE::set_month(value);
    return value;
}

uint8_t RTC_DS32xx::get_day() {
    uint8_t value=this->read_data(RTC_DS32xx::Register::rtc_date);
    RTCBASE::set_day(value);
    return value;
}

uint8_t RTC_DS32xx::get_hour() {
    uint8_t value=this->read_data(RTC_DS32xx::Register::rtc_hour);
    if(RTCBASE::get_hour_mode()==RTCBASE::HOUR_MODE::_24H_MODE)
        RTCBASE::set_hour(value);
    else {
        if(value > 12){
            value-=12;
            RTCBASE::set_hour(value,RTCBASE::DAY_APM::PM);
        } else{
            RTCBASE::set_hour(value,RTCBASE::DAY_APM::AM);
        }
    }
    return value;
}

uint8_t RTC_DS32xx::get_apm() {
    this->get_hour();
    return RTCBASE::get_apm();
}

uint8_t RTC_DS32xx::get_min() {
    uint8_t value=this->read_data(RTC_DS32xx::Register::rtc_min);
    RTCBASE::set_min(value);
    return value;
}

uint8_t RTC_DS32xx::get_sec() {
    uint8_t value=this->read_data(RTC_DS32xx::Register::rtc_sec);
    RTCBASE::set_sec(value);
    return value;
}

uint8_t RTC_DS32xx::get_date(uint16_t *y, uint8_t *m, uint8_t *d) {
    *y=this->get_year();
    *m=this->get_month();
    *d=this->get_day();
    return this->get_week();
}

uint8_t RTC_DS32xx::get_time(uint8_t *h, uint8_t *m, uint8_t *s) {
    *h=this->get_hour();
    *m=this->get_min();
    *s=this->get_sec();
    return RTCBASE::get_apm();
}












