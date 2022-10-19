/**
* @Author kokirika
* @Name RTCBASE
* @Date 2022-10-19
**/

#ifndef KOKIRIKA_RTCBASE_H
#define KOKIRIKA_RTCBASE_H

#include "sys.h"
#include "string"

class RTCBASE {
private:
    uint8_t  week_mode=WEEK_MODE::Monday_First;
    uint8_t  hour_mode=HOUR_MODE::_12H_MODE;
    void     hour_change();
    void     week_change();
public:
    enum WEEK_MODE{
        Monday_First,
        Sunday_First,
    };
    enum HOUR_MODE{
        _24H_MODE,
        _12H_MODE,
    };
    enum DAY_APM{
        AM,
        PM,
    };
    enum name_mode{
        EN,
        CH,
    };
    union Day_Name {
        enum:uint8_t {
            Mon,
            Tues,
            Wed,
            Thur,
            Fri,
            Sat,
            Sun,
        };
        enum:uint8_t {
            Monday,
            Tuesday,
            Wednesday,
            Thursday,
            Friday,
            Saturday,
            Sunday,
        };
    };

    uint16_t year{};
    uint8_t  month{};
    uint8_t  day{};
    uint8_t  week{};
    uint8_t  hour{};
    uint8_t  apm{};
    uint8_t  min{};
    uint8_t  sec{};
    uint8_t  name{};

    static uint8_t calculate_week(uint16_t ny,uint8_t nm,uint8_t nd,uint8_t weekmode=WEEK_MODE::Monday_First);
    virtual uint8_t get_week();
    virtual uint16_t get_year();
    virtual uint8_t get_month();
    virtual uint8_t get_day();
    virtual uint8_t get_hour();
    virtual uint8_t get_apm();
    virtual uint8_t get_min();
    virtual uint8_t get_sec();
    virtual uint8_t get_date(uint16_t *y,uint8_t *m,uint8_t *d);
    virtual uint8_t get_time(uint8_t *h,uint8_t *m,uint8_t *s);
    uint8_t get_week_mode() const;
    uint8_t get_hour_mode() const;

    virtual void set_year(uint16_t y);
    virtual void set_month(uint8_t m);
    virtual void set_day(uint8_t d);
    virtual void set_hour(uint8_t h,uint8_t ap);
    virtual void set_apm(uint8_t ap);
    virtual void set_min(uint8_t m);
    virtual void set_sec(uint8_t s);
    virtual void set_date(uint16_t y,uint8_t m,uint8_t d);
    virtual void set_time(uint8_t h,uint8_t m,uint8_t s,uint8_t ap);
    void set_week_mode(RTCBASE::WEEK_MODE mode);
    void set_hour_mode(RTCBASE::HOUR_MODE mode);

    uint8_t get_name() const;
    std::string get_name_str(name_mode mode =name_mode::EN) const;
};




#endif //KOKIRIKA_RTCBASE_H
