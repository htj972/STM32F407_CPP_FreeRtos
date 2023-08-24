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
    //名字 	    值 			报文流动方向 	描述
    M_RESERVED1	=0	,	//	禁止	保留
    M_CONNECT		,	//	客户端到服务端	客户端请求连接服务端
    M_CONNACK		,	//	服务端到客户端	连接报文确认
    M_PUBLISH		,	//	两个方向都允许	发布消息
    M_PUBACK		,	//	两个方向都允许	QoS 1消息发布收到确认
    M_PUBREC		,	//	两个方向都允许	发布收到（保证交付第一步）
    M_PUBREL		,	//	两个方向都允许	发布释放（保证交付第二步）
    M_PUBCOMP		,	//	两个方向都允许	QoS 2消息发布完成（保证交互第三步）
    M_SUBSCRIBE		,	//	客户端到服务端	客户端订阅请求
    M_SUBACK		,	//	服务端到客户端	订阅请求报文确认
    M_UNSUBSCRIBE	,	//	客户端到服务端	客户端取消订阅请求
    M_UNSUBACK		,	//	服务端到客户端	取消订阅报文确认
    M_PINGREQ		,	//	客户端到服务端	心跳请求
    M_PINGRESP		,	//	服务端到客户端	心跳响应
    M_DISCONNECT	,	//	客户端到服务端	客户端断开连接
    M_RESERVED2		,	//	禁止	保留
}_typdef_mqtt_message;


//连接成功服务器回应 20 02 00 00
//客户端主动断开连接 e0 00
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
    struct ip_addr rmtipaddr{};  	//远端ip地址
    IP4_ADDR(&rmtipaddr, ip1, ip2, ip3, ip4);
    return Connect(rmtipaddr,port);
}

bool MQTT::Connect(ip_addr rmtipaddr, uint16_t port) {
    if(this->tcp!= nullptr){
        return this->tcp->connect(rmtipaddr,port);
    }
    return false;
}

//连接服务器的打包函数
bool MQTT::config(char *ClientID,char *Username,char *Password){
    uint16_t ClientIDLen = strlen(ClientID);
    uint16_t UsernameLen = strlen(Username);
    uint16_t PasswordLen = strlen(Password);
    uint8_t cnt=3;
    uint8_t wait;
    uint16_t DataLen;
    //可变报头+Payload  每个字段包含两个字节的长度标识
    DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
    //固定报头
    //控制报文类型
    this->txbuf.clear();
    this->txbuf+=(char)0x10;		//MQTT Message Type CONNECT
    //剩余长度(不包括固定头部)
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf+=(char)encodedByte;
    }while ( DataLen > 0 );

    //可变报头
    //协议名
    this->txbuf+=(char) 0x00;        		// Protocol Name Length MSB
    this->txbuf+=(char) 0x04;        		// Protocol Name Length LSB
    this->txbuf+=(char) 'M';        	// ASCII Code for M
    this->txbuf+=(char) 'Q';        	// ASCII Code for Q
    this->txbuf+=(char) 'T';        	// ASCII Code for T
    this->txbuf+=(char) 'T';        	// ASCII Code for T
    //协议级别
    this->txbuf+=(char) 0x04;        		// MQTT Protocol version = 4
    //连接标志
    this->txbuf+=(char) 0xc2;        	// conn flags
    this->txbuf+=(char) 0x00;        		// Keep-alive Time Length MSB
    this->txbuf+=(char) 60;        	// Keep-alive Time Length LSB  60S心跳包

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
        wait=30;//等待3s时间
        while(wait--)
        {
            //CONNECT
            if(this->rxbuf[0]==parket_connetAck[0] && this->rxbuf[1]==parket_connetAck[1]) //连接成功
            {
                this->txbuf.clear();
                return true;//连接成功
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

//MQTT订阅/取消订阅数据打包函数
//topic       主题
//qos         消息等级
//whether     订阅/取消订阅请求包
bool MQTT::SubscribeTopic(char *topic,uint8_t qos,uint8_t whether){
    uint8_t cnt=10;
    uint8_t wait;
    uint16_t topiclen = strlen(topic);
    uint16_t DataLen = 2 + (topiclen+2) + (whether?1:0);//可变报头的长度（2字节）加上有效载荷的长度
    //固定报头
    //控制报文类型
    this->txbuf.clear();
    if (whether) this->txbuf+=(char)0x82; //消息类型和标志订阅
    else	this->txbuf+=(char)0xA2;    //取消订阅
    //剩余长度
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf+=(char)encodedByte;
    }while ( DataLen > 0 );
    //可变报头
    this->txbuf+=(char)0x00;		    //消息标识符 MSB
    this->txbuf+=(char)0x01;            //消息标识符 LSB
    //有效载荷
    this->txbuf+=(char)BYTE1(topiclen);//主题长度 MSB
    this->txbuf+=(char)BYTE0(topiclen);//主题长度 LSB
    this->txbuf.append(topic,topiclen);

    if(whether)
        this->txbuf+=(char)qos;//QoS级别
    while(cnt--)
    {
        rxbuf.clear();
        Send((char*)this->txbuf.data(),this->txbuf.length());
        wait=30;//等待3s时间
        while(wait--)
        {
            if(this->rxbuf[0]==parket_subAck[0] && this->rxbuf[1]==parket_subAck[1]) //订阅成功
            {
                this->txbuf.clear();
                return true;//订阅成功
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

//MQTT发布数据打包函数
//topic   主题
//message 消息
//qos     消息等级
bool MQTT::PublishData(char *topic, char *message, uint8_t qos){
    uint16_t topicLength = strlen(topic);
    uint16_t messageLength = strlen(message);
    static uint16_t id=0;
    uint16_t DataLen;
    //有效载荷的长度这样计算：用固定报头中的剩余长度字段的值减去可变报头的长度
    //QOS为0时没有标识符
    //数据长度             主题名   报文标识符   有效载荷
    if(qos)	DataLen = (2+topicLength) + 2 + messageLength;
    else	DataLen = (2+topicLength) + messageLength;

    //固定报头
    //控制报文类型
    this->txbuf.clear();
    this->txbuf+=(char)0x30;    // MQTT Message Type PUBLISH
    //剩余长度
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf+=(char)encodedByte;
    }while ( DataLen > 0 );
    this->txbuf+=(char)BYTE1(topicLength);//主题长度MSB
    this->txbuf+=(char)BYTE0(topicLength);//主题长度LSB
    this->txbuf.append(topic,topicLength);

    //报文标识符
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
