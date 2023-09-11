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
#include "tcp_client/TCP_Client_Class.h"

class ThingsBoard {
private:
    _USART_ *Debug;
    MQTT    *mqtt;
    uint16_t pack_size{};
    string SHA256;
    uint16_t size{};
    string title;
    string version;
    string jsonmessage;
     struct server{
        uint8_t ip[4]{};
        uint16_t port{};
    }mqtt_ser,tcp_ser;
    struct {
        string ClientID;
        string Username;
        string Password;
    }mqtt_user;
    MQTT::Subscribe request;//��ȡָ��
    MQTT::Subscribe upgrade;//��ȡ������Ϣ
    MQTT::Subscribe updata;//��ȡ�����ְ�����
    MQTT::Publish response;//��������
    MQTT::Publish telemetry;//��������
    MQTT::Publish getupdata;//��ȡ�����ְ�����
public:
    enum LINK_STATE:uint8_t {
        ALL_link_error=0x00,
        TCP_link_success=0x01,
        MQTT_link_success=0x02,
        ALL_link_TCP_link_success=0x03,
    };
    uint8_t  link_sata;
    uint16_t error_link_count{};
    bool     error_link_flag{};
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
    void PublishData(const string& message);
    void Getdatacheck();
    void GetVersion();
    void GetUpdata_pack(uint16_t pack,uint16_t len);
    void GetUpdata();

    bool TCP_data_check(TCP_Client_Class *tcp);
    static string TCP_data_process(string &data);

    void relink(TCP_Client_Class *tcp);
    void TCP_config(TCP_Client_Class *tcp,uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port);
    void mqtt_config(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port);
    void mqtt_config(const string& ClientID,const string& Username,const string& Password);

    uint8_t updata_step{};
};


#endif //KOKIRIKA_THINGSBOARD_H
