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
    this->request=MQTT::Subscribe("v1/devices/me/rpc/request/+");//�������� ��ȡָ��
    this->upgrade=MQTT::Subscribe("v1/devices/me/attributes/response/+");//��ȡ������Ϣ
    this->updata=MQTT::Subscribe("v2/fw/response/+/chunk/+");//��ȡ�����ְ�����

    this->response=MQTT::Publish("v1/devices/me/attributes/request/5");//�������� ��������
    //{"sharedKeys": "fw_version,fw_checksum_algorithm,fw_checksum,fw_size,fw_title,fw_version"}//������Ϣ
    this->telemetry=MQTT::Publish("v1/devices/me/telemetry");//�������� ��������
    this->getupdata=MQTT::Publish("v2/fw/request/1/chunk/");//�������� ��ȡ�����ְ�����
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
    ret = this->mqtt->SubscribeTopic(this->upgrade);
    //���ĸ�����Ϣ
    ret &= this->mqtt->SubscribeTopic(this->request);
    //���ĸ��·ְ�����
    ret &= this->mqtt->SubscribeTopic(this->updata);
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
//BEEP
extern _OutPut_ BEEP;

void ThingsBoard::Getdatacheck() {
    if(this->mqtt->available()){                              //���յ�����
        string sdata=this->mqtt->GetRxbuf();
        *this->Debug<<sdata<<"\r\n";
        this->mqtt->Message_analyze(sdata);//�������յ�������

        if(this->mqtt->check_topic(this->request)){
            cJSON *root = cJSON_Parse(this->mqtt->getMsg().data());
            //���json�Ƿ���ȷcJSON_GetErrorPtr
            if(root!= nullptr){
                //"method":"setValue"
                cJSON *method = cJSON_GetObjectItem(root,"method");
                if(method!= nullptr) {
                    if (std::string(method->valuestring) == "setValue") {
                        cJSON *item = cJSON_GetObjectItem(root,"params");
                        if(item!= nullptr) {
                            if (item->valueint==1) {
//                                BEEP.set(ON);
                            } else if (item->valueint==0) {
//                                BEEP.set(OFF);
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
                    }
                }
                cJSON_Delete(method);
            }
            cJSON_Delete(root);
        } else if(this->mqtt->check_topic(this->upgrade)){
            cJSON *root = cJSON_Parse(this->mqtt->getMsg().data());
            //���json�Ƿ���ȷcJSON_GetErrorPtr
            if(root!= nullptr){
                cJSON *item = cJSON_GetObjectItem(root,"sharedKeys");
                if(item!= nullptr) {
                    cJSON *fw_checksum = cJSON_GetObjectItem(item,"fw_checksum");
                    if(fw_checksum!= nullptr) {
                        this->SHA256=fw_checksum->valuestring;
                    }
                    cJSON_Delete(fw_checksum);
                    cJSON *fw_size = cJSON_GetObjectItem(item,"fw_size");
                    if(fw_size!= nullptr) {
                        this->size=fw_size->valueint;
                    }
                    cJSON_Delete(fw_size);
                    cJSON *fw_title = cJSON_GetObjectItem(item,"fw_title");
                    if(fw_title!= nullptr) {
                        this->title=fw_title->valuestring;
                    }
                    cJSON_Delete(fw_title);
                    cJSON *fw_version = cJSON_GetObjectItem(item,"fw_version");
                    if(fw_version!= nullptr) {
                        this->version=fw_version->valuestring;
                        this->updata_step=2;
                    }
                    cJSON_Delete(fw_version);
                }
                cJSON_Delete(item);
            }
            cJSON_Delete(root);
        } else if(this->mqtt->check_topic(this->updata)){
            cJSON *root = cJSON_Parse(this->mqtt->getMsg().data());
            //���json�Ƿ���ȷcJSON_GetErrorPtr
            if(root!= nullptr){
                cJSON *item = cJSON_GetObjectItem(root,"chunk");
                if(item!= nullptr) {
                    this->GetUpdata_pack(item->valueint,this->pack_size);
                }
                cJSON_Delete(item);
            }
            cJSON_Delete(root);
        }
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


