/**
* @Author kokirika
* @Name THINGSBOARD
* @Date 2023-06-09
*/

#include "ThingsBoard.h"
#include "lwip_comm/lwip_comm.h"
#include "delay.h"
#include "cJSON.h"
#include "Kstring.h"
#include "Out_In_Put.h"

void ThingsBoard::intel_link() {
    *this->Debug<<"lwIP Initing...\r\n";
    while(lwip_comm_init()!=0)
    {
        *this->Debug<<"lwIP Init failed!\r\n";
        delay_ms(1200);
//        my_mem_init(SRAMIN);		//初始化内部内存池
        *this->Debug<<"Retrying...\r\n";
    }
    *this->Debug<<"lwIP Init Successed\r\n";
    //等待DHCP获取
    *this->Debug<<"DHCP IP configing...\r\n";
    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
    {
        lwip_periodic_handle();
    }
    uint8_t speed;
    *this->Debug<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];
    speed=LAN8720_Get_Speed();//得到网速
    if(speed&1<<1)*this->Debug<<"Ethernet Speed:100M\r\n";
    else *this->Debug<<"Ethernet Speed:10M\r\n";
}

void ThingsBoard::topic_config() {
    this->request=MQTT::Subscribe("v1/devices/me/rpc/request/+");//订阅主题 获取指令
    this->upgrade=MQTT::Subscribe("v1/devices/me/attributes/response/+");//获取升级信息
    this->updata=MQTT::Subscribe("v2/fw/response/+/chunk/+");//获取升级分包数据

    this->response=MQTT::Publish("v1/devices/me/attributes/request/1");//发布主题 升级请求
    //{"sharedKeys": "fw_version,fw_checksum_algorithm,fw_checksum,fw_size,fw_title,fw_version"}//请求信息
    this->telemetry=MQTT::Publish("v1/devices/me/telemetry");//发布主题 发送数据
    this->getupdata=MQTT::Publish("v2/fw/request/1/chunk/");//发布主题 获取升级分包数据
}

bool ThingsBoard::Connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port) {
    return this->mqtt->Connect(ip1,ip2,ip3,ip4,port);
}

bool ThingsBoard::config(const string& ClientID, const string& Username, const string& Password) {
    return this->mqtt->config(ClientID,Username,Password);
}

bool ThingsBoard::SubscribeTopic() {
    return this->mqtt->SubscribeTopic(this->request);
}

void ThingsBoard::PublishData(const string &message, double value) {
    cJSON *root = cJSON_CreateObject();
    //添加键值对
    cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8(message).data(),value);
    //将JSON对象转化为字符串
    string buf=cJSON_Print(root);
    //删除JSON对象
    cJSON_Delete(root);

    *this->Debug<<buf<<"\r\n";
    this->mqtt->PublishData(telemetry,buf);
}
//BEEP
extern _OutPut_ BEEP;

void ThingsBoard::Getdatacheck() {
    if(this->mqtt->available()){                              //接收到数据
        this->mqtt->Message_analyze(this->mqtt->GetRxbuf());//分析接收到的数据

        if(this->mqtt->check_topic(this->request)){
            cJSON *root = cJSON_Parse(this->mqtt->getMsg().data());
            //检查json是否正确cJSON_GetErrorPtr
            if(root!= nullptr){
                cJSON *item = cJSON_GetObjectItem(root,"params");
                if(item!= nullptr) {
                    if (item->valueint==1) {
                        BEEP.set(ON);
                    } else if (item->valueint==0) {
                        BEEP.set(OFF);
                    }
                }
                cJSON_Delete(item);
            }
            cJSON_Delete(root);
        }
    }
}

void ThingsBoard::GetVersion() {
    this->mqtt->PublishData(this->response,
                            R"({"sharedKeys": "fw_version,
                                        fw_checksum_algorithm,fw_checksum,
                                        fw_size,fw_title,fw_version"})");
}

void ThingsBoard::GetUpdata_pack(uint16_t pack, uint16_t len) {
    this->mqtt->PublishData(this->getupdata.getTopic()+to_string(pack),
                            to_string(len));
}

void ThingsBoard::GetUpdata() {
    uint16_t pack_num= this->pack_size/1024;
    if(this->pack_size%1024!=0)pack_num++;
    for(uint16_t ii=0;ii<pack_num;ii++){
        this->GetUpdata_pack(ii,this->pack_size);
        delay_ms(100);
    }
}


