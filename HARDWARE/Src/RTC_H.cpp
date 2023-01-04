/**
* @Author kokirika
* @Name RTC_H
* @Date 2022-12-28
**/

#include "RTC_H.h"
#include "delay.h"

/*!
 * �ڲ�RTC  ��ʼ��
 * @return �Ƿ�ɹ�
 */
bool RTC_H ::init()
{
    u16 retry=0X1FFF;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
    PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������

    if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5060)		//�Ƿ��һ������?
    {
        RCC_LSEConfig(RCC_LSE_ON);//LSE ����
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
        {
            retry++;
            delay_ms(10);
        }
        if(retry==0)return false;		//LSE ����ʧ��.

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��
        RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��

        RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��(1~0X7F)
        RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��(0~7FFF)
        RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
        RTC_Init(&RTC_InitStructure);

        RTC_WriteBackupRegister(RTC_BKP_DR0,0x5060);	//����Ѿ���ʼ������
    }
    return true;
}
/*!
 * �ڲ�RTC  ����ʱ��
 * @return �Ƿ�ɹ�
 */
ErrorStatus RTC_H ::Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
    RTC_TimeStruct.RTC_Hours=hour;
    RTC_TimeStruct.RTC_Minutes=min;
    RTC_TimeStruct.RTC_Seconds=sec;
    RTC_TimeStruct.RTC_H12=ampm;
    return RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);
}
/*!
 * �ڲ�RTC  ��������
 * @return �Ƿ�ɹ�
 */
ErrorStatus RTC_H ::Set_Date(u8 year,u8 month,u8 date,u8 week)
{
    RTC_DateStruct.RTC_Date=date;
    RTC_DateStruct.RTC_Month=month;
    RTC_DateStruct.RTC_WeekDay=week;
    RTC_DateStruct.RTC_Year=year;
    return RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);
}

uint8_t RTC_H::get_week() {
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
    RTCBASE::set_date(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
    return RTCBASE::get_week();
}

uint16_t RTC_H::get_year() {
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
    RTCBASE::set_year(RTC_DateStruct.RTC_Year);
    return RTC_DateStruct.RTC_Year;
}

uint8_t RTC_H::get_month() {
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
    RTCBASE::set_month(RTC_DateStruct.RTC_Month);
    return RTC_DateStruct.RTC_Month;
}

uint8_t RTC_H::get_day() {
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
    RTCBASE::set_day(RTC_DateStruct.RTC_Date);
    return RTC_DateStruct.RTC_Date;
}

uint8_t RTC_H::get_hour() {
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTCBASE::set_hour(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_H12);
    return RTCBASE::get_hour();
}

uint8_t RTC_H::get_apm() {
    this->get_hour();
    return RTCBASE::get_apm();
}

uint8_t RTC_H::get_min() {
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTCBASE::set_min(RTC_TimeStruct.RTC_Minutes);
    return RTC_TimeStruct.RTC_Minutes;
}

uint8_t RTC_H::get_sec() {
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTCBASE::set_sec(RTC_TimeStruct.RTC_Seconds);
    return RTC_TimeStruct.RTC_Seconds;
}

uint8_t RTC_H::get_date(uint16_t *y, uint8_t *m, uint8_t *d) {
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
    RTCBASE::set_date(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
    *y=RTC_DateStruct.RTC_Year;
    *m=RTC_DateStruct.RTC_Month;
    *d=RTC_DateStruct.RTC_Date;
    return RTCBASE::get_week();
}

uint8_t RTC_H::get_time(uint8_t *h, uint8_t *m, uint8_t *s) {
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTCBASE::set_hour(RTC_TimeStruct.RTC_Hours);
    RTCBASE::set_min(RTC_TimeStruct.RTC_Minutes);
    RTCBASE::set_sec(RTC_TimeStruct.RTC_Seconds);
    *h=RTC_TimeStruct.RTC_Hours;
    *m=RTC_TimeStruct.RTC_Minutes;
    *s=RTC_TimeStruct.RTC_Seconds;
    return RTCBASE::get_apm();
}

void RTC_H::set_year(uint16_t y) {
    RTCBASE::set_year(y);
    get_date((uint16_t*)&RTC_DateStruct.RTC_Year,&RTC_DateStruct.RTC_Month,&RTC_DateStruct.RTC_Date);
    Set_Date(y,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,this->get_week());
}

void RTC_H::set_month(uint8_t m) {
    RTCBASE::set_month(m);
    get_date((uint16_t*)&RTC_DateStruct.RTC_Year,&RTC_DateStruct.RTC_Month,&RTC_DateStruct.RTC_Date);
    Set_Date(RTC_DateStruct.RTC_Year,m,RTC_DateStruct.RTC_Date,this->get_week());
}

void RTC_H::set_day(uint8_t d) {
    RTCBASE::set_day(d);
    get_date((uint16_t*)&RTC_DateStruct.RTC_Year,&RTC_DateStruct.RTC_Month,&RTC_DateStruct.RTC_Date);
    Set_Date(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,d,this->get_week());
}

void RTC_H::set_hour(uint8_t h, uint8_t ap) {
    RTCBASE::set_hour(h, ap);
    get_time(&RTC_TimeStruct.RTC_Hours,&RTC_TimeStruct.RTC_Minutes,&RTC_TimeStruct.RTC_Seconds);
    Set_Time(h,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,ap);
}

void RTC_H::set_hour(uint8_t h) {
    RTCBASE::set_hour(h);
    RTC_TimeStruct.RTC_H12=RTCBASE::get_apm();
    get_time(&RTC_TimeStruct.RTC_Hours,&RTC_TimeStruct.RTC_Minutes,&RTC_TimeStruct.RTC_Seconds);
    Set_Time(h,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_TimeStruct.RTC_H12);
}

void RTC_H::set_apm(uint8_t ap) {
    RTCBASE::set_apm(ap);
    RTC_TimeStruct.RTC_H12=get_time(&RTC_TimeStruct.RTC_Hours,&RTC_TimeStruct.RTC_Minutes,&RTC_TimeStruct.RTC_Seconds);
    Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,ap);
}

void RTC_H::set_min(uint8_t m) {
    RTCBASE::set_min(m);
    RTC_TimeStruct.RTC_H12=get_time(&RTC_TimeStruct.RTC_Hours,&RTC_TimeStruct.RTC_Minutes,&RTC_TimeStruct.RTC_Seconds);
    Set_Time(RTC_TimeStruct.RTC_Hours,m,RTC_TimeStruct.RTC_Seconds,RTC_TimeStruct.RTC_H12);
}

void RTC_H::set_sec(uint8_t s) {
    RTCBASE::set_sec(s);
    RTC_TimeStruct.RTC_H12=get_time(&RTC_TimeStruct.RTC_Hours,&RTC_TimeStruct.RTC_Minutes,&RTC_TimeStruct.RTC_Seconds);
    Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,s,RTC_TimeStruct.RTC_H12);
}

void RTC_H::set_date(uint16_t y, uint8_t m, uint8_t d) {
    RTCBASE::set_year(y);
    RTCBASE::set_month(m);
    RTCBASE::set_day(d);
    Set_Date(y, m, d,RTCBASE::get_week());
}

void RTC_H::set_time(uint8_t h, uint8_t m, uint8_t s, uint8_t ap) {
    RTCBASE::set_hour(h);
    RTCBASE::set_min(m);
    RTCBASE::set_sec(s);
    RTCBASE::set_apm(ap);
    Set_Time(h, m, s, ap);
}

void RTC_H::set_time(uint8_t h, uint8_t m, uint8_t s) {
    RTCBASE::set_hour(h);
    RTCBASE::set_min(m);
    RTCBASE::set_sec(s);
    Set_Time(h, m, s,RTCBASE::get_apm());
}


