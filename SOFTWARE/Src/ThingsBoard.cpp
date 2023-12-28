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
//        my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
        *this->Debug<<"Retrying...\r\n";
    }
    *this->Debug<<"lwIP Init Successed\r\n";
    //�ȴ�DHCP��ȡ
    *this->Debug<<"DHCP IP configing...\r\n";
    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
    {
        lwip_periodic_handle();
    }
    uint8_t speed;
    *this->Debug<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];
    speed=LAN8720_Get_Speed();//�õ�����
    if(speed&1<<1)*this->Debug<<"Ethernet Speed:100M\r\n";
    else *this->Debug<<"Ethernet Speed:10M\r\n";
}

void ThingsBoard::topic_config() {
    this->request=mqtt_base::Subscribe("v1/devices/me/rpc/request/+");//�������� ��ȡָ��
    this->upgrade=mqtt_base::Subscribe("v1/devices/me/attributes/response/+");//��ȡ������Ϣ
    this->updata=mqtt_base::Subscribe("v2/fw/response/+/chunk/+");//��ȡ�����ְ�����

    this->response=mqtt_base::Publish("v1/devices/me/attributes/request/1");//�������� ��������
    //{"sharedKeys": "fw_version,fw_checksum_algorithm,fw_checksum,fw_size,fw_title"}//������Ϣ
    this->telemetry=mqtt_base::Publish("v1/devices/me/telemetry");//�������� ��������
    this->getupdata=mqtt_base::Publish("v2/fw/request/1/chunk/");//�������� ��ȡ�����ְ�����
}

bool ThingsBoard::Connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port) {
    return this->mqtt->Connect(ip1,ip2,ip3,ip4,port);
}

bool ThingsBoard::config(const string& ClientID, const string& Username, const string& Password) {
    return this->mqtt->config(ClientID,Username,Password);
}

bool ThingsBoard::SubscribeTopic() {
    //�������ݻ�ȡ
    bool ret;
    ret = this->mqtt->SubscribeTopic(this->request);
    //���ĸ�����Ϣ
//    ret &= this->mqtt->SubscribeTopic(this->upgrade);
//    //���ĸ��·ְ�����
//    ret &= this->mqtt->SubscribeTopic(this->updata);
    //���ض��Ľ��
    return ret;
}

void ThingsBoard::PublishData(const string &message, double value) {
    cJSON *root = cJSON_CreateObject();
    //��Ӽ�ֵ��
    cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8(message).data(),value);
    //��JSON����ת��Ϊ�ַ���
    string buf=cJSON_Print(root);
    //ɾ��JSON����
    cJSON_Delete(root);

    *this->Debug<<buf<<"\r\n";
    this->mqtt->PublishData(telemetry,buf);
}

void ThingsBoard::PublishData(const string& message) {
    *this->Debug<<message<<"\r\n";
    this->mqtt->PublishData(telemetry,Kstring::GBK_to_utf8(message));
}
//BEEP
extern _OutPut_ OUT;

bool ThingsBoard::Getdatacheck() {
    bool ret = false;
    if(this->mqtt->available()){                              //���յ�����
        string sdata=this->mqtt->GetRxbuf();
        *this->Debug<<sdata<<"\r\n";

        if(this->mqtt->check_topic(this->request)){
            cJSON *root = cJSON_Parse(sdata.data());
            //���json�Ƿ���ȷcJSON_GetErrorPtr
            if(root!= nullptr){
                cJSON *method = cJSON_GetObjectItem(root,"method");
                //{"method":"write_node","params":[{"deviceId":"self","setOUT":1}]}
                if(method!= nullptr) {
                    if(std::string(method->valuestring)== "write_node") {
                        cJSON *item = cJSON_GetObjectItem(root, "params");
                        if (item != nullptr && item->type == cJSON_Array) {
                            int arraySize = cJSON_GetArraySize(item);
                            for (int i = 0; i < arraySize; i++) {
                                cJSON *paramItem = cJSON_GetArrayItem(item, i);
                                if (paramItem != nullptr) {
                                    // ���� "deviceId"
                                    cJSON *deviceId = cJSON_GetObjectItem(paramItem, "deviceId");
                                    if (deviceId != nullptr) {
                                        if (std::string(deviceId->valuestring) == "self") {
                                            // ���� "setOUT"
                                            cJSON *setOUT = cJSON_GetObjectItem(paramItem, "setOUT");
                                            if (setOUT != nullptr) {
                                                if (setOUT->valueint == 1) {
                                                    OUT.set(ON);
                                                } else if (setOUT->valueint == 0) {
                                                    OUT.set(OFF);
                                                }
                                                ret = true;
                                            }
                                            cJSON_Delete(setOUT);
                                        }
                                    }
                                    cJSON_Delete(deviceId);
                                }
                                cJSON_Delete(paramItem);
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
//            //���json�Ƿ���ȷcJSON_GetErrorPtr
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
//            //���json�Ƿ���ȷcJSON_GetErrorPtr
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
    return ret;
}

void ThingsBoard::GetVersion() {
    if(this->updata_step==3) {
        //{"sharedKeys": "fw_version,fw_checksum_algorithm,fw_checksum,fw_size,fw_title"}
        string buf=R"({"sharedKeys":"fw_version,fw_checksum_algorithm,fw_checksum,fw_size,fw_title"})";
        *this->Debug<<buf<<"\r\n";
        this->mqtt->PublishData(this->telemetry,buf);
        this->updata_step = 1;
    }
    //��ӡ���
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
                            to_string(len),0);
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
    return this->mqtt->PHY_status();
}

bool ThingsBoard::intel_islink() {
    return this->mqtt->islink();
}

void ThingsBoard::inter_unlink() {
    this->mqtt->Disconnect();
}




