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
    string SHA256;
    uint16_t size{};
    string title;
    string version;
    MQTT::Subscribe request;//��ȡָ��
    MQTT::Subscribe upgrade;//��ȡ������Ϣ
    MQTT::Subscribe updata;//��ȡ�����ְ�����
    MQTT::Publish response;//��������
    MQTT::Publish telemetry;//��������
    MQTT::Publish getupdata;//��ȡ�����ְ�����
public:
    ~ThingsBoard() = default;
    ThingsBoard(_USART_ *Debug,MQTT *mqtt):Debug(Debug),mqtt(mqtt){
        this->topic_config();
    }
    static bool PHY_islink();
    bool intel_islink();
    void intel_link();
    void inter_unlink();
    void topic_config();
    bool Connect(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port=1883);
    bool config(const string& ClientID,const string& Username,const string& Password);
    bool SubscribeTopic();
    void PublishData(const string& message,double value);
    void Getdatacheck();
    void GetVersion();
    void GetUpdata_pack(uint16_t pack,uint16_t len);
    void GetUpdata();

    uint8_t updata_step{};
};


#endif //KOKIRIKA_THINGSBOARD_H
