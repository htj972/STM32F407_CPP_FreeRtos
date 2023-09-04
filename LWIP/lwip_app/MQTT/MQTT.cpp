/**
* @Author kokirika
* @Name MQTT
* @Date 2023-06-09
*/
#include <cstring>
#include <utility>
#include "MQTT.h"
#include "delay.h"


typedef enum
{
    //���� 	    ֵ 			������������ 	����
    M_RESERVED1	=0	,	//	��ֹ	����
    M_CONNECT		,	//	�ͻ��˵������	�ͻ����������ӷ����
    M_CONNACK		,	//	����˵��ͻ���	���ӱ���ȷ��
    M_PUBLISH		,	//	������������	������Ϣ
    M_PUBACK		,	//	������������	QoS 1��Ϣ�����յ�ȷ��
    M_PUBREC		,	//	������������	�����յ�����֤������һ����
    M_PUBREL		,	//	������������	�����ͷţ���֤�����ڶ�����
    M_PUBCOMP		,	//	������������	QoS 2��Ϣ������ɣ���֤������������
    M_SUBSCRIBE		,	//	�ͻ��˵������	�ͻ��˶�������
    M_SUBACK		,	//	����˵��ͻ���	����������ȷ��
    M_UNSUBSCRIBE	,	//	�ͻ��˵������	�ͻ���ȡ����������
    M_UNSUBACK		,	//	����˵��ͻ���	ȡ�����ı���ȷ��
    M_PINGREQ		,	//	�ͻ��˵������	��������
    M_PINGRESP		,	//	����˵��ͻ���	������Ӧ
    M_DISCONNECT	,	//	�ͻ��˵������	�ͻ��˶Ͽ�����
    M_RESERVED2		,	//	��ֹ	����
}_typdef_mqtt_message;


//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00
const char parket_connetAck[] = {0x20,0x02,0x00,0x00};
const char parket_disconnet[] = {0xe0,0x00};
const char parket_heart[] = {0xc0,0x00};
const char parket_heart_reply[] = {0xc0,0x00};
const char parket_subAck[] = {0x90,0x03};


uint8_t MQTT::BYTE0(uint16_t data_temp)
{
    return (data_temp&0x00ff)>>0;
}

uint8_t MQTT::BYTE1(uint16_t data_temp)
{
    return (data_temp&0xff00)>>8;
}

MQTT::MQTT(TCP_BASE *TCP) {
    this->Init(TCP);
}

void MQTT::Init(TCP_BASE *TCP){
    this->tcp = TCP;
    this->tcp->upload_extern_fun(this);
}

bool MQTT::Connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port) {
    struct ip_addr rmtipaddr{};  	//Զ��ip��ַ
    IP4_ADDR(&rmtipaddr, ip1, ip2, ip3, ip4);
    return Connect(rmtipaddr,port);
}

bool MQTT::Connect(ip_addr rmtipaddr, uint16_t port) {
    if(this->tcp!= nullptr){
        return this->tcp->connect(rmtipaddr,port);
    }
    return false;
}

//���ӷ������Ĵ������
bool MQTT::config(char *ClientID,char *Username,char *Password){
    uint16_t ClientIDLen = strlen(ClientID);
    uint16_t UsernameLen = strlen(Username);
    uint16_t PasswordLen = strlen(Password);
    uint8_t cnt=3;
    uint8_t wait;
    uint16_t DataLen;
    //�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
    DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
    //�̶���ͷ
    //���Ʊ�������
    this->txbuf.clear();
    this->txbuf+=(char)0x10;		//MQTT Message Type CONNECT
    //ʣ�೤��(�������̶�ͷ��)
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf+=(char)encodedByte;
    }while ( DataLen > 0 );

    //�ɱ䱨ͷ
    //Э����
    this->txbuf+=(char) 0x00;        		// Protocol Name Length MSB
    this->txbuf+=(char) 0x04;        		// Protocol Name Length LSB
    this->txbuf+=(char) 'M';        	// ASCII Code for M
    this->txbuf+=(char) 'Q';        	// ASCII Code for Q
    this->txbuf+=(char) 'T';        	// ASCII Code for T
    this->txbuf+=(char) 'T';        	// ASCII Code for T
    //Э�鼶��
    this->txbuf+=(char) 0x04;        		// MQTT Protocol version = 4
    //���ӱ�־
    this->txbuf+=(char) 0xc2;        	// conn flags
    this->txbuf+=(char) 0x00;        		// Keep-alive Time Length MSB
    this->txbuf+=(char) 60;        	// Keep-alive Time Length LSB  60S������

    this->txbuf+=(char) BYTE1(ClientIDLen);// Client ID length MSB
    this->txbuf+=(char) BYTE0(ClientIDLen);// Client ID length LSB
    this->txbuf.append(ClientID,ClientIDLen);

    if(UsernameLen > 0)
    {
        this->txbuf+= BYTE1(UsernameLen);		//username length MSB
        this->txbuf+= BYTE0(UsernameLen);    	//username length LSB
        this->txbuf.append(Username,UsernameLen);
    }

    if(PasswordLen > 0)
    {
        this->txbuf+= BYTE1(PasswordLen);		//password length MSB
        this->txbuf+= BYTE0(PasswordLen);    	//password length LSB
        this->txbuf.append(Password,PasswordLen);

    }
    while(cnt--)
    {
        this->Send((char*)this->txbuf.data(),this->txbuf.length());//this->txlen
        wait=30;//�ȴ�3sʱ��
        while(wait--)
        {
            //CONNECT
            if(this->rxbuf[0]==parket_connetAck[0] && this->rxbuf[1]==parket_connetAck[1]) //���ӳɹ�
            {
                this->txbuf.clear();
                return true;//���ӳɹ�
            }
            delay_ms(100);
        }
    }
    this->txbuf.clear();
    return false;
}

bool MQTT::config(const std::string& ClientID, const std::string& Username, const std::string& Password) {
    return this->config((char*)ClientID.data(),(char*)Username.data(),(char*)Password.data());
}

//MQTT����/ȡ���������ݴ������
//topic       ����
//qos         ��Ϣ�ȼ�
//whether     ����/ȡ�����������
bool MQTT::SubscribeTopic(char *topic,uint8_t qos,uint8_t whether){
    uint8_t cnt=10;
    uint8_t wait;
    uint16_t topiclen = strlen(topic);
    uint16_t DataLen = 2 + (topiclen+2) + (whether?1:0);//�ɱ䱨ͷ�ĳ��ȣ�2�ֽڣ�������Ч�غɵĳ���
    //�̶���ͷ
    //���Ʊ�������
    this->txbuf.clear();
    if (whether) this->txbuf+=(char)0x82; //��Ϣ���ͺͱ�־����
    else	this->txbuf+=(char)0xA2;    //ȡ������
    //ʣ�೤��
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf+=(char)encodedByte;
    }while ( DataLen > 0 );
    //�ɱ䱨ͷ
    this->txbuf+=(char)0x00;		    //��Ϣ��ʶ�� MSB
    this->txbuf+=(char)0x01;            //��Ϣ��ʶ�� LSB
    //��Ч�غ�
    this->txbuf+=(char)BYTE1(topiclen);//���ⳤ�� MSB
    this->txbuf+=(char)BYTE0(topiclen);//���ⳤ�� LSB
    this->txbuf.append(topic,topiclen);

    if(whether)
        this->txbuf+=(char)qos;//QoS����
    while(cnt--)
    {
        rxbuf.clear();
        Send((char*)this->txbuf.data(),this->txbuf.length());
        wait=30;//�ȴ�3sʱ��
        while(wait--)
        {
            if(this->rxbuf[0]==parket_subAck[0] && this->rxbuf[1]==parket_subAck[1]) //���ĳɹ�
            {
                this->txbuf.clear();
                return true;//���ĳɹ�
            }
            delay_ms(100);
        }
    }
    this->txbuf.clear();
    return false;
}

bool MQTT::SubscribeTopic(const std::string& topic, uint8_t qos, uint8_t whether) {
    return this->SubscribeTopic((char*)topic.data(),qos,whether);
}

bool MQTT::SubscribeTopic(const MQTT::Subscribe& subscribe) {
    return this->SubscribeTopic(subscribe.getTopic(),subscribe.getQos(),subscribe.getWhether());
}

//MQTT�������ݴ������
//topic   ����
//message ��Ϣ
//qos     ��Ϣ�ȼ�
bool MQTT::PublishData(char *topic, char *message, uint8_t qos){
    uint16_t topicLength = strlen(topic);
    uint16_t messageLength = strlen(message);
    static uint16_t id=0;
    uint16_t DataLen;
    //��Ч�غɵĳ����������㣺�ù̶���ͷ�е�ʣ�೤���ֶε�ֵ��ȥ�ɱ䱨ͷ�ĳ���
    //QOSΪ0ʱû�б�ʶ��
    //���ݳ���             ������   ���ı�ʶ��   ��Ч�غ�
    if(qos)	DataLen = (2+topicLength) + 2 + messageLength;
    else	DataLen = (2+topicLength) + messageLength;

    //�̶���ͷ
    //���Ʊ�������
    this->txbuf.clear();
    this->txbuf+=(char)0x30;    // MQTT Message Type PUBLISH
    //ʣ�೤��
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf+=(char)encodedByte;
    }while ( DataLen > 0 );
    this->txbuf+=(char)BYTE1(topicLength);//���ⳤ��MSB
    this->txbuf+=(char)BYTE0(topicLength);//���ⳤ��LSB
    this->txbuf.append(topic,topicLength);

    //���ı�ʶ��
    if(qos)
    {
        this->txbuf+=(char)BYTE1(id);
        this->txbuf+=(char)BYTE0(id);
        id++;
    }
    this->txbuf.append(message,messageLength);
    Send((char*)this->txbuf.data(),this->txbuf.length());
    return this->txbuf.length();
}

bool MQTT::PublishData(const std::string& topic, const std::string& message, uint8_t qos) {
    return this->PublishData((char*)topic.data(),(char*)message.data(),qos);
}

bool MQTT::PublishData(const MQTT::Publish &publish) {
    return this->PublishData(publish.getTopic(),publish.getMessage(),publish.getQos());
}

bool MQTT::PublishData(const Publish& publish, const std::string &message) {
    return this->PublishData(publish.getTopic(),message,publish.getQos());
}

bool MQTT::PublishData(const Publish& publish, const std::string &message, uint8_t qos) {
    return this->PublishData(publish.getTopic(),message,qos);
}

void MQTT::SendHeart(){
    Send((char *)parket_heart,sizeof(parket_heart));
}

void MQTT::Disconnect(){
    this->tcp->close();
}

void MQTT::close() {
    Send((char *)parket_disconnet,sizeof(parket_disconnet));
}

void MQTT::Session(std::string str){
    this->rxbuf = std::move(str);
}

void MQTT::Send(char *buf,uint16_t len){
    this->tcp->send_data(buf,len);
}

void MQTT::Callback(std::string str) {
    Session(std::move(str));
}

void MQTT::Send(const std::string& buf) {
    this->tcp->send_data((char*)buf.data(),buf.length());
}

std::string MQTT::GetRxbuf() {
    std::string str = this->rxbuf;
    this->rxbuf.clear();
    return str;
}

bool MQTT::available() {
    return !this->rxbuf.empty();
}

bool MQTT::islink() {
    return this->tcp->islink();
}

void MQTT::Clear() {
    this->rxbuf.clear();
}
















/*********************************************END OF FILE********************************************/
