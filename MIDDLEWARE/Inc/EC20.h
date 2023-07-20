/**
* @Author kokirika
* @Name EC20
* @Date 2023-07-18
*/
#ifndef KOKIRIKA_EC20_H
#define KOKIRIKA_EC20_H

#include "USART.h"

class EC20 {
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
private:
    _USART_ *USART{};
    _USART_ *Debug_USART{};
    void (*Debug)(const string& str){};
    bool  new_flag{};
    string getstring{};
    static bool Compare(const string& target,const string& data);
    bool sendcom(const string& CMD,const string& REC,uint8_t delay_time=5);
    static string getAPN_Name(const APN& apn);
    void debug(const string& str);
public:
    explicit EC20(USART_TypeDef* USARTx,int32_t bound=115200);
    explicit EC20(_USART_* USARTx);
    EC20()=default;
    ~EC20();
    bool init(USART_TypeDef* USARTx,int32_t bound=115200);
    bool init(_USART_* USARTx);
    bool init();

    bool getrdy();
    void setdebug(void (*debug)(const string& str));
    void setdebug(_USART_* USARTx);

    bool attest();
    bool getcpin();
    bool getcsq();
    bool getcreg();
    bool getcgreg();
    bool setqicgsp(APN apn=APN_UNINET);
    bool setqideact();
    bool setqiact();
    bool Register(APN apn=APN_UNINET);

    bool mqttopen(uint8_t id,const string& ip,uint16_t port);
    bool mqttconn(uint8_t id,const string& clientid,const string& username,const string& password);
    bool mqttpub(uint8_t id,const string& topic,const string& message);
    bool mqttclose(uint8_t id);
    bool mqttdisc(uint8_t id);

//    void write(const char *str,uint16_t len) override;
//    void write(uint8_t *str,uint16_t len) override;
//    void write(string String) override;
};


#endif //KOKIRIKA_EC20_H
