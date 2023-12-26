/**
* @Author kokirika
* @Name THINGSBOARD
* @Date 2023-06-09
*/

#ifndef KOKIRIKA_THINGSBOARD_H
#define KOKIRIKA_THINGSBOARD_H

#include "sys.h"
#include "USART.h"
#include "mqtt_base.h"

class ThingsBoard {
private:
    _USART_ *Debug;
    mqtt_base    *mqtt;
    uint16_t pack_size{};
    string SHA256;
    uint16_t size{};
    string title;
    string version;
    string jsonmessage;
    mqtt_base::Subscribe request;//��ȡָ��
    mqtt_base::Subscribe upgrade;//��ȡ������Ϣ
    mqtt_base::Subscribe updata;//��ȡ�����ְ�����
    mqtt_base::Publish response;//��������
    mqtt_base::Publish telemetry;//��������
    mqtt_base::Publish getupdata;//��ȡ�����ְ�����
public:
    ~ThingsBoard() = default;
    ThingsBoard(_USART_ *Debug,mqtt_base *mqtt):Debug(Debug),mqtt(mqtt){
        this->topic_config();
    }
    bool PHY_islink();
    bool intel_islink();
    void intel_link();
    void inter_unlink();
    void topic_config();
    bool Connect(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port=1883);
    bool config(const string& ClientID,const string& Username,const string& Password);
    bool SubscribeTopic();
    void PublishData(const string& message,double value);
    void PublishData(const string& message);
    bool Getdatacheck();
    void GetVersion();
    void GetUpdata_pack(uint16_t pack,uint16_t len);
    void GetUpdata();

    uint8_t updata_step{};
};


#endif //KOKIRIKA_THINGSBOARD_H
