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
    mqtt_base::Subscribe request;//获取指令
    mqtt_base::Subscribe upgrade;//获取升级信息
    mqtt_base::Subscribe updata;//获取升级分包数据
    mqtt_base::Publish response;//升级请求
    mqtt_base::Publish telemetry;//发送数据
    mqtt_base::Publish getupdata;//获取升级分包数据
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
