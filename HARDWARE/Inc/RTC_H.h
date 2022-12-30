/**
* @Author kokirika
* @Name RTC_H
* @Date 2022-12-28
**/

#ifndef KOKIRIKA_RTC_H_H
#define KOKIRIKA_RTC_H_H

#include "RTCBASE.h"

class RTC_H :public RTCBASE{
private:
//    NVIC_InitTypeDef     NVIC_InitStructure{};
    RTC_InitTypeDef      RTC_InitStructure{};
    RTC_TimeTypeDef 	 RTC_TimeStruct{};
    RTC_DateTypeDef		 RTC_DateStruct{};
protected:
    ErrorStatus Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
    ErrorStatus Set_Date(u8 year,u8 month,u8 date,u8 week);
public:
    RTC_H()=default;
    bool init();
    uint8_t get_week() override;
    uint16_t get_year() override;
    uint8_t get_month() override;
    uint8_t get_day() override;
    uint8_t get_hour() override;
    uint8_t get_apm() override;
    uint8_t get_min() override;
    uint8_t get_sec() override;
    uint8_t get_date(uint16_t *y,uint8_t *m,uint8_t *d) override;
    uint8_t get_time(uint8_t *h,uint8_t *m,uint8_t *s) override;

    void set_year(uint16_t y) override;
    void set_month(uint8_t m) override;
    void set_day(uint8_t d) override;
    void set_hour(uint8_t h,uint8_t ap) override;
    void set_hour(uint8_t h) override;
    void set_apm(uint8_t ap) override;
    void set_min(uint8_t m) override;
    void set_sec(uint8_t s) override;
    void set_date(uint16_t y,uint8_t m,uint8_t d) override;
    void set_time(uint8_t h,uint8_t m,uint8_t s,uint8_t ap) override;
    void set_time(uint8_t h,uint8_t m,uint8_t s) override;
};


#endif //KOKIRIKA_RTC_H_H
