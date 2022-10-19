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
        rtc_date    =0x04,
        rtc_mon     =0x05,
        rtc_year    =0x06,
        rtc_week    =0x03,
    };
    uint8_t  Addr;
    uint8_t read_data(uint8_t cmd) const;
public:
    Software_IIC *IIC;

    explicit RTC_DS32xx(Software_IIC *IICx,uint8_t addr=0XD0);
    void RTC_Read();


};


#endif //KOKIRIKA_RTC_DS32XX_H
