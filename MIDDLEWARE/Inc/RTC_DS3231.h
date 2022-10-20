/**
* @Author kokirika
* @Name RTC_DS32XX
* @Date 2022-10-19
**/
#ifndef KOKIRIKA_RTC_DS32XX_H
#define KOKIRIKA_RTC_DS32XX_H

#include "RTCBASE.h"
#include "IIC.h"

class RTC_DS32xx: public RTCBASE{
private:
    enum Register{
        rtc_sec     =0x00,
        rtc_min     =0x01,
        rtc_hour    =0x02,
        rtc_week    =0x03,
        rtc_date    =0x04,
        rtc_mon     =0x05,
        rtc_year    =0x06,
    };
    uint8_t  Addr{};
    uint8_t read_data(uint8_t cmd) const;
    void    write_data(uint8_t cmd,uint8_t data) const;
    void    set_week();
public:
    Software_IIC *IIC{};

    explicit RTC_DS32xx(Software_IIC *IICx,uint8_t addr=0XD0);
    RTC_DS32xx()=default;
    void init(Software_IIC *IICx,uint8_t addr=0XD0);

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

};


#endif //KOKIRIKA_RTC_DS32XX_H
