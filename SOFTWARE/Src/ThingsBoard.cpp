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
    bool ret=this->mqtt->config(ClientID,Username,Password);
    this->mqtt->Clear();
    return ret;
}

bool ThingsBoard::SubscribeTopic() {
    //订阅数据获取
    bool ret;
    ret = this->mqtt->SubscribeTopic(this->request);
    //订阅更新信息
//    ret &= this->mqtt->SubscribeTopic(this->upgrade);
//    //订阅更新分包数据
//    ret &= this->mqtt->SubscribeTopic(this->updata);
    //返回订阅结果
    return ret;
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

void ThingsBoard::PublishData(const string& message) {
//    *this->Debug<<"MQTT:"<<message<<"\r\n";
    this->mqtt->PublishData(telemetry,message);
}
//BEEP
extern _OutPut_ OUT1,OUT2;

void ThingsBoard::Getdatacheck() {
    if(this->mqtt->available()){                              //接收到数据
        string sdata=this->mqtt->GetRxbuf();
//        if(sdata.empty())return;
        *this->Debug<<sdata<<"\r\n";
        this->mqtt->Message_analyze(sdata);//分析接收到的数据

//        this->mqtt->Message_analyze(this->mqtt->GetRxbuf());//分析接收到的数据

        if(this->mqtt->check_topic(this->request)){
            cJSON *root = cJSON_Parse(this->mqtt->getMsg().data());
            //检查json是否正确cJSON_GetErrorPtr
            if(root!= nullptr){
                //"method":"setValue"
                cJSON *method = cJSON_GetObjectItem(root,"method");
                if(method!= nullptr) {
                    if (std::string(method->valuestring) == "setinsect") {
                        cJSON *item = cJSON_GetObjectItem(root,"params");
                        if(item!= nullptr) {
                            if (item->valueint==1) {
                                OUT1.set(ON);
                            } else if (item->valueint==0) {
                                OUT1.set(OFF);
                            }
                        }
                        cJSON_Delete(item);
                    } else if(std::string(method->valuestring) == "setbird_repellency") {
                        cJSON *item = cJSON_GetObjectItem(root,"params");
                        if(item!= nullptr) {
                            if (item->valueint==1) {
                                OUT2.set(ON);
                            } else if (item->valueint==0) {
                                OUT2.set(OFF);
                            }
                        }
                        cJSON_Delete(item);
                    } else if(std::string(method->valuestring) == "updateFirmware") {
                        cJSON *item = cJSON_GetObjectItem(root,"params");
                        if(item!= nullptr) {
                            if (item->valueint==1) {
                                this->updata_step=3;
                            }
                        }
                        cJSON_Delete(item);
                    }
                }
                cJSON_Delete(method);
            }
            cJSON_Delete(root);
        }
//        else if(this->mqtt->check_topic(this->upgrade)){
//            cJSON *root = cJSON_Parse(this->mqtt->getMsg().data());
//            //检查json是否正确cJSON_GetErrorPtr
//            if(root!= nullptr){
//                cJSON *item = cJSON_GetObjectItem(root,"sharedKeys");
//                if(item!= nullptr) {
//                    cJSON *fw_checksum = cJSON_GetObjectItem(item,"fw_checksum");
//                    if(fw_checksum!= nullptr) {
//                        this->SHA256=fw_checksum->valuestring;
//                    }
//                    cJSON_Delete(fw_checksum);
//                    cJSON *fw_size = cJSON_GetObjectItem(item,"fw_size");
//                    if(fw_size!= nullptr) {
//                        this->size=fw_size->valueint;
//                    }
//                    cJSON_Delete(fw_size);
//                    cJSON *fw_title = cJSON_GetObjectItem(item,"fw_title");
//                    if(fw_title!= nullptr) {
//                        this->title=fw_title->valuestring;
//                    }
//                    cJSON_Delete(fw_title);
//                    cJSON *fw_version = cJSON_GetObjectItem(item,"fw_version");
//                    if(fw_version!= nullptr) {
//                        this->version=fw_version->valuestring;
//                        this->updata_step=2;
//                    }
//                    cJSON_Delete(fw_version);
//                }
//                cJSON_Delete(item);
//            }
//            cJSON_Delete(root);
//        } else if(this->mqtt->check_topic(this->updata)){
//            cJSON *root = cJSON_Parse(this->mqtt->getMsg().data());
//            //检查json是否正确cJSON_GetErrorPtr
//            if(root!= nullptr){
//                cJSON *item = cJSON_GetObjectItem(root,"chunk");
//                if(item!= nullptr) {
//                    this->GetUpdata_pack(item->valueint,this->pack_size);
//                }
//                cJSON_Delete(item);
//            }
//            cJSON_Delete(root);
//        }
    }
}

void ThingsBoard::GetVersion() {
    if(this->updata_step==3) {
        //{"sharedKeys": "fw_version,fw_checksum_algorithm,fw_checksum,fw_size,fw_title,fw_version"}
        *this->Debug<<R"({"sharedKeys": "fw_version,)"
                      R"(fw_checksum_algorithm,fw_checksum,)"
                      R"(fw_size,fw_title,fw_version"})"<<"\r\n";
        this->mqtt->PublishData(this->telemetry,
                                R"({"sharedKeys":"fw_version,fw_checksum_algorithm,fw_checksum,fw_size,fw_title,fw_version"})");
        this->updata_step = 1;
    }
        //打印结果
    if(this->updata_step==2) {
        *this->Debug << "SHA256:" << this->SHA256 << "\r\n";
        *this->Debug << "size:" << this->size << "\r\n";
        *this->Debug << "title:" << this->title << "\r\n";
        *this->Debug << "version:" << this->version << "\r\n";
        this->updata_step = 0;
    }

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

bool ThingsBoard::PHY_islink() {
    return CheckLinkStatus()==LAN8720_LINKED;
}

bool ThingsBoard::intel_islink() {
    return this->mqtt->islink();
}

void ThingsBoard::inter_unlink() {
    return this->mqtt->Disconnect();
}

bool ThingsBoard::TCP_data_check(TCP_Client_Class *tcp) {
    bool ret= false;
    if(tcp->available()>0){

        string redate=tcp->read_data();
        //删除前4位
        redate.erase(0,4);
        *this->Debug<<redate;
        cJSON *root = cJSON_Parse(redate.data());
        //检查json是否正确cJSON_GetErrorPtr
        if(root!= nullptr) {
            //"method":"setValue"
            cJSON *method = cJSON_GetObjectItem(root, "switchId");
            if (method != nullptr) {
                if (std::string(method->valuestring) == "K1") {
                    cJSON *item = cJSON_GetObjectItem(root, "status");
                    if (item != nullptr) {
                        if (item->valueint == 1) {
                            OUT1.set(ON);
                        } else if (item->valueint == 0) {
                            OUT1.set(OFF);
                        }
                        ret = true;
                    }
                    cJSON_Delete(item);
                } else if (std::string(method->valuestring) == "K2") {
                    cJSON *item = cJSON_GetObjectItem(root, "status");
                    if (item != nullptr) {
                        if (item->valueint == 1) {
                            OUT2.set(ON);
                        } else if (item->valueint == 0) {
                            OUT2.set(OFF);
                        }
                        ret = true;
                    }
                    cJSON_Delete(item);
                }
            }
            cJSON_Delete(method);
        }
        cJSON_Delete(root);
    }
    return ret;
}

string ThingsBoard::TCP_data_process(string &data) {
    string str;
    str.append(R"({"sensor":)");
    str.append(data);
    str.append(R"(,"switch":[{)");
    str.append(R"("switchId":"K1","status":)");
    str.append(to_string(OUT1.get()));
    str.append(R"(},{"switchId":"K2","status":)");
    str.append(to_string(OUT2.get()));
    str.append("}]}");
    return str;
}

void ThingsBoard::relink(TCP_Client_Class *tcp) {
    if(!this->intel_islink()) {
        if((this->link_sata&ThingsBoard::MQTT_link_success)!=ThingsBoard::MQTT_link_success){
            this->Connect(this->mqtt_ser.ip[0],this->mqtt_ser.ip[1],this->mqtt_ser.ip[2],this->mqtt_ser.ip[3],this->mqtt_ser.port);
            this->config(this->mqtt_user.ClientID,this->mqtt_user.Username,this->mqtt_user.Password);
            *this->Debug<<"订阅结果"<<this->SubscribeTopic()<<"\r\n";
        }
        else {
            this->inter_unlink();
            this->link_sata &=~ ThingsBoard::LINK_STATE::MQTT_link_success;
        }
        delay_ms(100);
        if(this->intel_islink()){
            this->link_sata|=ThingsBoard::MQTT_link_success;
        }
    }
    if(!tcp->islink()){
        if((this->link_sata&ThingsBoard::TCP_link_success)!=ThingsBoard::TCP_link_success){
            tcp->connect(this->tcp_ser.ip[0],this->tcp_ser.ip[1],this->tcp_ser.ip[2],this->tcp_ser.ip[3],this->tcp_ser.port);
            *this->Debug<<"relink"<<"\r\n";
        }
        else {
            tcp->close();
            this->link_sata &=~ ThingsBoard::LINK_STATE::TCP_link_success;
        }
        delay_ms(100);
        if(tcp->islink()){
            this->link_sata|=ThingsBoard::TCP_link_success;
        }
    }
}

void ThingsBoard::TCP_config(TCP_Client_Class *tcp,uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port) {
    if(!tcp->islink()){
        this->tcp_ser.ip[0]=ip1;
        this->tcp_ser.ip[1]=ip2;
        this->tcp_ser.ip[2]=ip3;
        this->tcp_ser.ip[3]=ip4;
        this->tcp_ser.port=port;
        tcp->connect(ip1,ip2,ip3,ip4,port);
    }
}

void ThingsBoard::mqtt_config(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port) {
    this->mqtt_ser.ip[0]=ip1;
    this->mqtt_ser.ip[1]=ip2;
    this->mqtt_ser.ip[2]=ip3;
    this->mqtt_ser.ip[3]=ip4;
    this->mqtt_ser.port=port;
}

void ThingsBoard::mqtt_config(const string &ClientID, const string &Username, const string &Password) {
    if(!this->intel_islink()) {
        this->mqtt_user.ClientID=ClientID;
        this->mqtt_user.Username=Username;
        this->mqtt_user.Password=Password;
        this->Connect(this->mqtt_ser.ip[0],this->mqtt_ser.ip[1],this->mqtt_ser.ip[2],this->mqtt_ser.ip[3],this->mqtt_ser.port);
        this->config(ClientID,Username,Password);
        *this->Debug<<"订阅结果"<<this->SubscribeTopic()<<"\r\n";
    }
}




