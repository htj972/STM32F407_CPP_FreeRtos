/**
* @Author kokirika
* @Name THINGSBOARD
* @Date 2023-06-09
*/

#ifndef KOKIRIKA_THINGSBOARD_H
#define KOKIRIKA_THINGSBOARD_H

#include "sys.h"
#include "USART.h"
#include "MQTT/MQTT.h"

class ThingsBoard {
private:
    _USART_ *Debug;
    MQTT    *mqtt;
    uint16_t pack_size{};
    MQTT::Subscribe request;
    MQTT::Subscribe upgrade;
    MQTT::Subscribe updata;
    MQTT::Publish response;
    MQTT::Publish telemetry;
    MQTT::Publish getupdata;
public:
    ~ThingsBoard() = default;
    ThingsBoard(_USART_ *Debug,MQTT *mqtt):Debug(Debug),mqtt(mqtt){
        this->topic_config();
    }
    void intel_link();
    void topic_config();
    bool Connect(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port=1883);
    bool config(const string& ClientID,const string& Username,const string& Password);
    bool SubscribeTopic();
    void PublishData(const string& message,double value);
    void Getdatacheck();
    void GetVersion();
    void GetUpdata_pack(uint16_t pack,uint16_t len);
    void GetUpdata();

};


#endif //KOKIRIKA_THINGSBOARD_H
