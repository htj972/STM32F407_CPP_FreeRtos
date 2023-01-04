/**
* @Author kokirika
* @Name RTCBASE
* @Date 2022-10-19
**/

#include "RTCBASE.h"



void RTCBASE::hour_change() {
    if(this->hour_mode==RTCBASE::HOUR_MODE::_12H_MODE) {
        if (this->hour > 12) {
            this->apm = RTCBASE::DAY_APM::PM;
            this->hour -= 12;
        }
        else
            this->apm = RTCBASE::DAY_APM::AM;
    }
    else if(this->hour_mode==RTCBASE::HOUR_MODE::_24H_MODE)
        if((this->apm==RTCBASE::DAY_APM::PM)&&(this->hour < 12)){
            this->apm = RTCBASE::DAY_APM::AM;
            this->hour += 12;
        }
}

uint8_t RTCBASE::week_change() {
    this->week=RTCBASE::calculate_week(this->year,this->month,this->day,this->week_mode);
    return this->week;
}

uint8_t RTCBASE::calculate_week(uint16_t ny,uint8_t nm,uint8_t nd,uint8_t weekmode) {
    uint16_t  week_t;
    uint8_t y , c, m, d = nd;
    if (nm <= 2)
    {
        c = ( ny - 1 ) / 100;
        y = ( ny - 1 ) % 100;
        m = nm + 12;
    }
    else
    {
        c = ny / 100;
        y = ny % 100;
        m = nm;
    }
    week_t = y + y / 4 + c / 4 - 2 * c + 26 * ( m + 1 ) / 10 + d - 1; //蔡勒公式
    week_t = week_t >= 0 ? ( week_t % 7 ) : ( week_t % 7 + 7 ); //iWeek为负时取模
    if(weekmode==RTCBASE::WEEK_MODE::Monday_First){
        if ( week_t == 0 ) //星期日不作为一周的第一天
        {
            week_t = 7;
        }
    }
    else if(weekmode==RTCBASE::WEEK_MODE::Sunday_First){
        week_t++;
    }
    return week_t;
}

uint8_t RTCBASE::calculate_week(RTCBASE *own, uint8_t weekmode) {
    return RTCBASE::calculate_week(own->get_year(),own->get_month(),
                                   own->get_day(),weekmode);
}

uint8_t RTCBASE::get_week() {
    this->week_change();
    return this->week;
}

uint16_t RTCBASE::get_year() {
    return this->year;
}

uint8_t RTCBASE::get_month() {
    return this->month;
}

uint8_t RTCBASE::get_day() {
    return this->day;
}

uint8_t RTCBASE::get_hour() {
    return this->hour;
}

uint8_t RTCBASE::get_apm() {
    this->hour_change();
    return this->apm;
}

uint8_t RTCBASE::get_min() {
    return this->min;
}

uint8_t RTCBASE::get_sec() {
    return this->sec;
}

uint8_t RTCBASE::get_date(uint16_t *y,uint8_t *m,uint8_t *d){
    *y= this->get_year();
    *m= this->get_month();
    *d= this->get_day();
    return this->get_week();
}

uint8_t RTCBASE::get_time(uint8_t *h,uint8_t *m,uint8_t *s) {
    *h= this->get_hour();
    *m= this->get_min();
    *s= this->get_sec();
    this->hour_change();
    return this->get_apm();
}

uint8_t RTCBASE::get_week_mode() const {
    return this->week_mode;
}

uint8_t RTCBASE::get_hour_mode() const {
    return this->hour_mode;
}

void RTCBASE::set_hour_mode(RTCBASE::HOUR_MODE mode) {
    this->hour_mode=mode;
    this->hour_change();
}

void RTCBASE::set_week_mode(RTCBASE::WEEK_MODE mode) {
    this->week_mode=mode;
    this->week_change();
}

void RTCBASE::set_year(uint16_t y) {
    this->year=y;
}

void RTCBASE::set_month(uint8_t m) {
    this->month=m;
}

void RTCBASE::set_day(uint8_t d) {
    this->day=d;
}

void RTCBASE::set_hour(uint8_t h,uint8_t ap) {
    this->hour=h;
    this->apm=ap;
    hour_change();
}

void RTCBASE::set_hour(uint8_t h) {
    this->hour=h;
    if(this->hour>=13)
        this->apm=RTCBASE::DAY_APM::PM;
    else
        this->apm=RTCBASE::DAY_APM::AM;
}

void RTCBASE::set_apm(uint8_t ap) {
    this->apm=ap;
    if(this->hour_mode==RTCBASE::HOUR_MODE::_12H_MODE)
    {
        if (this->hour > 12) {
            this->hour -= 12;
        }
    }
}

void RTCBASE::set_min(uint8_t m) {
    this->min=m;
}

void RTCBASE::set_sec(uint8_t s) {
    this->sec=s;
}

void RTCBASE::set_date(uint16_t y, uint8_t m, uint8_t d) {
    this->set_year(y);
    this->set_month(m);
    this->set_day(d);
}

void RTCBASE::set_time(uint8_t h, uint8_t m, uint8_t s,uint8_t ap) {
    this->set_hour(h,ap);
    this->set_min(m);
    this->set_sec(s);
}

void RTCBASE::set_time(uint8_t h, uint8_t m, uint8_t s) {
    this->set_hour(h);
    this->set_min(m);
    this->set_sec(s);
}

uint8_t RTCBASE::get_name() {
    if(this->week_mode==RTCBASE::WEEK_MODE::Monday_First)
        this->name=RTCBASE::Day_Name::Mon+this->get_week()-1;
    else{
        uint8_t week_n= this->get_week()-1;
        if ( week_n == 0 ) week_n = 7;
        this->name=RTCBASE::Day_Name::Mon+week_n-1;
    }
    return this->name;
}

std::string RTCBASE::get_name_str(name_mode mode) {
    if(mode == RTCBASE::name_mode::EN)
        switch(this->get_name())
        {
            case Day_Name::Mon  :return "Monday";
            case Day_Name::Tues :return "Tuesday";
            case Day_Name::Wed  :return "Wednesday";
            case Day_Name::Thur :return "Thursday";
            case Day_Name::Fri  :return "Friday";
            case Day_Name::Sat  :return "Saturday";
            case Day_Name::Sun  :return "Sunday";
            default:return "name error";
        }
    else if(mode == RTCBASE::name_mode::CH)
        switch(this->get_name())
        {
            case Day_Name::Mon  :return "星期一";
            case Day_Name::Tues :return "星期二";
            case Day_Name::Wed  :return "星期三";
            case Day_Name::Thur :return "星期四";
            case Day_Name::Fri  :return "星期五";
            case Day_Name::Sat  :return "星期六";
            case Day_Name::Sun  :return "星期日";
            default:return "日期错误";
        }
    return "name error";
}

void RTCBASE::set_Initial_time() {
    uint8_t h,s,m;
    this->get_time(&h,&m,&s);
    this->Initial_time=h*3600+m*60+s;
}

uint32_t RTCBASE::get_run_time() {
    uint8_t h,s,m;
    this->get_time(&h,&m,&s);
    return (h*3600+m*60+s)-this->Initial_time;
}

void RTCBASE::get_run_time(uint8_t *h, uint8_t *m, uint8_t *s) {
    uint8_t sh,ss,sm;
    this->get_time(&sh,&sm,&ss);
    *h=(sh*3600+sm*60+ss-this->Initial_time)/3600;
    *m=(sh*3600+sm*60+ss-this->Initial_time)%3600/60;
    *s=(sh*3600+sm*60+ss-this->Initial_time)%60;
}






