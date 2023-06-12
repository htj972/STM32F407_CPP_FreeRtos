/**
* @Author kokirika
* @Name MQTT
* @Date 2023-06-09
*/
#ifndef KOKIRIKA_MQTT_H
#define KOKIRIKA_MQTT_H
#include "sys.h"
#include "tcp_client/tcp_base.h"

class MQTT :public Call_Back{
private:
    std::string rxbuf{};
    uint16_t rxlen{};
    std::string txbuf{};
    uint16_t txlen{};
    TCP_BASE *tcp{};
    static uint8_t BYTE0(uint16_t data_temp);
    static uint8_t BYTE1(uint16_t data_temp);

    void Send(char *buf,uint16_t len);
    void Send(const std::string& buf);
public:
    void Init(TCP_BASE *TCP);
    uint8_t Connect(char *ClientID,char *Username,char *Password);
    uint8_t SubscribeTopic(char *topic,uint8_t qos,uint8_t whether);
    uint8_t PublishData(char *topic, char *message, uint8_t qos);
    void SendHeart();
    void Disconnect();
    void Session(std::string str);
    void Callback(std::string str) override;
};


#endif //KOKIRIKA_MQTT_H


/*
DEMO：
	if(AT_WIFI_init()==WIFI_GET_DATA_SUCCEED)printf("WIFI_GET_DATA_SUCCEED\r\n");
	else	printf("WIFI_GET_DATA_ERROR\r\n");
	WIFI_LINK();
	WIFI_socket_init(Socket_Client,Socket_TCP,"47.110.139.57",1883);//47.110.139.57
	WIFI_DATA_Channel_ON();
	_mqtt.Init((uint8_t )wifi_get_buffer,sizeof((uint8_t )wifi_get_buffer),(uint8_t )wifi_send_bufer,sizeof((uint8_t )wifi_send_bufer),&wifi_get_buffer_len);
//	WIFI_Socket_send_data("hello word");
	if(_mqtt.Connect("zpwl","zpwl","20190325")!=0)
		printf("mqtt_init OK\r\n");
	else
		printf("mqtt_init ERROR\r\n");


	if(_mqtt.SubscribeTopic("test_G",0,1)!=0)
		printf("SubscribeTopic OK\r\n");
	else
		printf("SubscribeTopic error\r\n");

	_mqtt.PublishData("test_p","hello_word\r\n",0);
	wifi_get_buffer_len=0;

	while(1)
		TIM_ADD_software_struct(&BB,600,_mqtt.SendHeart);
		len=_mqtt.Session(str);
		if(len)
		{
			//printf("GET_Data: %s \r\n",str);
			{	//json数据处理
				cJSON  *cjson = cJSON_Parse((char*)str);
				if(!cJSON_GetErrorPtr())
				{
					char *a2 = cJSON_GetObjectItem(cjson, "id")->valuestring;
					char	a1 = cJSON_GetObjectItem(cjson, "sata")->valueint;
					printf("CJSON:a1:%d,a2:%s\r\n",a1,a2);
					if(a1==1)RUN_LED_SET(ON);
					if(a1==0)RUN_LED_SET(OFF);
					_mqtt.PublishData("test_p","ok\r\n",0);
					cJSON_Delete(cjson);
				}
			}
		}

*/



