/**
* @Author kokirika
* @Name EC20
* @Date 2023-07-18
*/
#ifndef KOKIRIKA_EC20_H
#define KOKIRIKA_EC20_H

#include "USART.h"
#include "Timer.h"
#include "mqtt_base.h"
#include "Out_In_Put.h"

class EC20 :public mqtt_base,public Call_Back{
public:
     enum APN:uint8_t {
        APN_CMNET =0,
        APN_CMWAP ,
        APN_UNINET,
        APN_UNIWAP,
        APN_3GWAP ,
        APN_3GNET ,
        APN_CTWAP ,
        APN_CTNET ,
     };
    struct {
        string UTC;
        string latitude;
        string longitude;
        string HDOP;
        string altitude;
        string fix;
        string cog;
        string spkm;
        string spkn;
        string date;
        string nsat;
    }GPS_D;
protected:
    Timer *TIMERX{};
    _USART_ *USART{};
    _USART_ *Debug_USART{};
    _OutPut_ *RST_Pin{};
    string getstring{};
    string Otherstring{};
    string AT_get{};
    uint16_t reveive_len{};
    uint16_t freetime_t{};
    uint16_t freetime{};
    uint8_t _4G_RSSI{};
    uint8_t GPRS_RSSI{};
    bool Link_s{};
private:
    bool  new_flag{};
    bool CallBack_flag{};
    bool init_flag{};
    static bool Compare(const string& target,const string& data);
    bool sendcom(const string& CMD,const string& REC,uint8_t delay_time=50);
    static string getAPN_Name(const APN& apn);
    void debug(const string& str);
    void Call_back_set(bool ON_OFF);
    void Call_back_set(bool ON_OFF,bool init);
public:
    explicit EC20(USART_TypeDef* USARTx,int32_t bound=115200);
    explicit EC20(_USART_* USARTx);
    EC20()=default;
    ~EC20();
    bool init(USART_TypeDef* USARTx,int32_t bound=115200);
    bool init(_USART_* USARTx);
    bool init();
    void set_freetime(uint16_t sfreetime);
    string read_data();

    bool getrdy();
    void setdebug(_USART_* USARTx);

    bool reset();
    bool attest();
    bool getcpin();
    bool getcsq();
    bool getcreg();
    bool getcgreg();
    bool setqicgsp(APN apn=APN_UNINET);
    bool setqideact();
    bool setqiact();
    bool Register(APN apn=APN_UNINET);
    bool setGPS(bool EN=true);
    bool getGPS();

    bool mqttopen(uint8_t id,const string& ip,uint16_t port);
    bool mqttconn(uint8_t id,const string& clientid,const string& username,const string& password);
//    bool mqttconn(uint8_t id,const string& clientid,const string& token);
    bool mqttpub(uint8_t id,const string& topic,const string& message);
    bool mqttclose(uint8_t id);
    bool mqttdisc(uint8_t id);
    bool mqttsub(uint8_t id,const string& topic,uint8_t qos=0);
    bool mqttunsub(uint8_t id,const string& topic);

    bool Connect(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port) override;
    bool config(const std::string& ClientID,const std::string& Username,const std::string& Password) override;
    bool SubscribeTopic(const Subscribe& subscribe) override;
    bool PublishData(const Publish& publish) override;
    bool PublishData(const Publish& publish,const std::string& Message) override;
    bool PublishData(const std::string& publish,const std::string& Message, uint8_t qos) override;
    bool available() override;
    bool islink() override;
    std::string GetRxbuf() override;
    void Disconnect() override;
    bool PHY_status() override;

    //串口数据回调
    void Link_RST_Pin(_OutPut_ *RST_Pinx);
    void Link_UART_CALLback();
    void Link_TIMER_CALLback(Timer *TIMX);
    void Callback(int, char **gdata) override;
    void Receive_data();

    bool RSSI_Status();
    uint8_t get_4G_RSSI() const;
    bool get_Link_Status() const;



//    void write(const char *str,uint16_t len) override;
//    void write(uint8_t *str,uint16_t len) override;
//    void write(string String) override;

    EC20& operator<<(const string& Str)
    {
        this->USART->write(Str);
        return *this;
    }


};


#endif //KOKIRIKA_EC20_H
