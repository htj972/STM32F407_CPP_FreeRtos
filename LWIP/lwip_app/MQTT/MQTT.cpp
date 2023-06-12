/**
* @Author kokirika
* @Name ${PACKAGE_NAME}
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

void MQTT::Init(TCP_BASE *TCP){
    this->tcp = TCP;
    this->tcp->upload_extern_fun(this);
}

//���ӷ������Ĵ������
uint8_t MQTT::Connect(char *ClientID,char *Username,char *Password){
    uint16_t ClientIDLen = strlen(ClientID);
    uint16_t UsernameLen = strlen(Username);
    uint16_t PasswordLen = strlen(Password);
    uint8_t cnt=3;
    uint8_t wait;
    uint16_t DataLen;
    this->txlen=0;
    //�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
    DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
    //�̶���ͷ
    //���Ʊ�������
    this->txbuf[this->txlen++] = 0x10;		//MQTT Message Type CONNECT
    //ʣ�೤��(�������̶�ͷ��)
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf[this->txlen++] = encodedByte;
    }while ( DataLen > 0 );

    //�ɱ䱨ͷ
    //Э����
    this->txbuf[this->txlen++] = 0;        		// Protocol Name Length MSB
    this->txbuf[this->txlen++] = 4;        		// Protocol Name Length LSB
    this->txbuf[this->txlen++] = 'M';        	// ASCII Code for M
    this->txbuf[this->txlen++] = 'Q';        	// ASCII Code for Q
    this->txbuf[this->txlen++] = 'T';        	// ASCII Code for T
    this->txbuf[this->txlen++] = 'T';        	// ASCII Code for T
    //Э�鼶��
    this->txbuf[this->txlen++] = 4;        		// MQTT Protocol version = 4
    //���ӱ�־
    this->txbuf[this->txlen++] = 0xc2;        	// conn flags
    this->txbuf[this->txlen++] = 0;        		// Keep-alive Time Length MSB
    this->txbuf[this->txlen++] = 60;        	// Keep-alive Time Length LSB  60S������

    this->txbuf[this->txlen++] = BYTE1(ClientIDLen);// Client ID length MSB
    this->txbuf[this->txlen++] = BYTE0(ClientIDLen);// Client ID length LSB
    memcpy(&this->txbuf[this->txlen],ClientID,ClientIDLen);
    this->txlen += ClientIDLen;

    if(UsernameLen > 0)
    {
        this->txbuf[this->txlen++] = BYTE1(UsernameLen);		//username length MSB
        this->txbuf[this->txlen++] = BYTE0(UsernameLen);    	//username length LSB
        memcpy(&this->txbuf[this->txlen],Username,UsernameLen);
        this->txlen += UsernameLen;
    }

    if(PasswordLen > 0)
    {
        this->txbuf[this->txlen++] = BYTE1(PasswordLen);		//password length MSB
        this->txbuf[this->txlen++] = BYTE0(PasswordLen);    	//password length LSB
        memcpy(&this->txbuf[this->txlen],Password,PasswordLen);
        this->txlen += PasswordLen;
    }
    while(cnt--)
    {
        Send((char*)this->txbuf.data(),this->txlen);//this->txlen
        wait=30;//�ȴ�3sʱ��
        while(wait--)
        {
            //CONNECT
            if(this->rxbuf[0]==parket_connetAck[0] && this->rxbuf[1]==parket_connetAck[1]) //���ӳɹ�
            {
                return 1;//���ӳɹ�
            }
            delay_ms(100);
        }
    }
    return 0;
}



//MQTT����/ȡ���������ݴ������
//topic       ����
//qos         ��Ϣ�ȼ�
//whether     ����/ȡ�����������
uint8_t MQTT::SubscribeTopic(char *topic,uint8_t qos,uint8_t whether){
    uint8_t cnt=10;
    uint8_t wait;
    uint16_t topiclen = strlen(topic);
    uint16_t DataLen = 2 + (topiclen+2) + (whether?1:0);//�ɱ䱨ͷ�ĳ��ȣ�2�ֽڣ�������Ч�غɵĳ���
    this->txlen=0;
    //�̶���ͷ
    //���Ʊ�������
    if(whether) this->txbuf[this->txlen++] = 0x82; //��Ϣ���ͺͱ�־����
    else	this->txbuf[this->txlen++] = 0xA2;    //ȡ������
    //ʣ�೤��
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf[this->txlen++] = encodedByte;
    }while ( DataLen > 0 );
    //�ɱ䱨ͷ
    this->txbuf[this->txlen++] = 0;				//��Ϣ��ʶ�� MSB
    this->txbuf[this->txlen++] = 0x01;           //��Ϣ��ʶ�� LSB
    //��Ч�غ�
    this->txbuf[this->txlen++] = BYTE1(topiclen);//���ⳤ�� MSB
    this->txbuf[this->txlen++] = BYTE0(topiclen);//���ⳤ�� LSB
    memcpy(&this->txbuf[this->txlen],topic,topiclen);
    this->txlen += topiclen;

    if(whether)
    {
        this->txbuf[this->txlen++] = qos;//QoS����
    }
    while(cnt--)
    {
        rxbuf.clear();
        Send((char*)this->txbuf.data(),this->txlen);
        wait=30;//�ȴ�3sʱ��
        while(wait--)
        {
            if(this->rxbuf[0]==parket_subAck[0] && this->rxbuf[1]==parket_subAck[1]) //���ĳɹ�
            {
                return 1;//���ĳɹ�
            }
            delay_ms(100);
        }
    }
    //if(cnt) return 1;	//���ĳɹ�
    return 0;
}

//MQTT�������ݴ������
//topic   ����
//message ��Ϣ
//qos     ��Ϣ�ȼ�
uint8_t MQTT::PublishData(char *topic, char *message, uint8_t qos){
    uint16_t topicLength = strlen(topic);
    uint16_t messageLength = strlen(message);
    static uint16_t id=0;
    uint16_t DataLen;
    this->txlen=0;
    //��Ч�غɵĳ����������㣺�ù̶���ͷ�е�ʣ�೤���ֶε�ֵ��ȥ�ɱ䱨ͷ�ĳ���
    //QOSΪ0ʱû�б�ʶ��
    //���ݳ���             ������   ���ı�ʶ��   ��Ч�غ�
    if(qos)	DataLen = (2+topicLength) + 2 + messageLength;
    else	DataLen = (2+topicLength) + messageLength;

    //�̶���ͷ
    //���Ʊ�������
    this->txbuf[this->txlen++] = 0x30;    // MQTT Message Type PUBLISH
    //ʣ�೤��
    do
    {
        uint8_t encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        // if there are more data to encode, set the top bit of this byte
        if ( DataLen > 0 )
            encodedByte = encodedByte | 128;
        this->txbuf[this->txlen++] = encodedByte;
    }while ( DataLen > 0 );
    this->txbuf[this->txlen++] = BYTE1(topicLength);//���ⳤ��MSB
    this->txbuf[this->txlen++] = BYTE0(topicLength);//���ⳤ��LSB
    memcpy(&this->txbuf[this->txlen],topic,topicLength);//��������
    this->txlen += topicLength;

    //���ı�ʶ��
    if(qos)
    {
        this->txbuf[this->txlen++] = BYTE1(id);
        this->txbuf[this->txlen++] = BYTE0(id);
        id++;
    }
    memcpy(&this->txbuf[this->txlen],message,messageLength);
    this->txlen += messageLength;
    Send((char*)this->txbuf.data(),this->txlen);
    return this->txlen;
}

void MQTT::SendHeart(){
    Send((char *)parket_heart,sizeof(parket_heart));
}

void MQTT::Disconnect(){
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



/*********************************************END OF FILE********************************************/
