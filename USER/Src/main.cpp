#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "RS485.h"
#include "malloc.h"
#include "lwip_comm/lwip_comm.h"
#include "Communication.h"
#include "udp/UDP_Class.h"
#include "cJSON.h"
#include "tcp_client/TCP_Client_Class.h"
#include "MQTT/MQTT.h"
#include "Kstring.h"




//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define TASK1_TASK_PRIO		2
//�����ջ��С
#define TASK1_STK_SIZE 		512
//������
TaskHandle_t Task1Task_Handler;
//������
[[noreturn]] void task1_task(void *pvParameters);

//�������ȼ�
#define TASK2_TASK_PRIO		2
//�����ջ��С
#define TASK2_STK_SIZE 		512*2
//������
TaskHandle_t Task2Task_Handler;
//������
[[noreturn]] void task2_task(void *pvParameters);

extern u32 lwip_localtime;		//lwip����ʱ�������,��λ:ms
//����ָʾ��
class T_led_:public _OutPut_,public Call_Back,public Timer{
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override{
        this->change();
        lwip_setup();
    };
}led(GPIOE5,TIM6,100);//����ָʾ�ƶ�ʱ��

_OutPut_ run (GPIOE6),BEEP (GPIOE4,HIGH);//����ָʾ��
_OutPut_ OUT1(GPIOE0),OUT2(GPIOE1);            //���

//Timer_queue tIMS(TIM7,50000);                 //��ʱ������

_USART_ DEBUG(USART1);                          //���Դ���
_USART_ Lora(USART6);                           //Lora����

Communication SUN(USART3,GPIOB15,TIM7,100);//modbusͨ��

TCP_Client_Class  tcp_mq;//TCPͨ��
MQTT    mqtt_demo(&tcp_mq);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��

//    BEEP.flicker(100,250,2);//��������ʾ

    delay_ms(1000);//��ʱ1s
    SUN.RS485::config(GPIOD8,GPIOD9);//����RS485 GPIO����

//    Kstring asdsad="����";
////    asdsad.append("����");
//    string ret=asdsad.GBK_to_utf8();
//    //16���ƴ�ӡRET
//    for (char i : ret) {
//        DEBUG.print("0x%02X ",i);
//    }

    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,          //������
                (const char*    )"start_task",           //��������
                (uint16_t       )START_STK_SIZE,     //�����ջ��С
                (void*          )nullptr,            //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}





//task1������
[[noreturn]] void task1_task(void *pvParameters)//alignas(8)
{
    string str;
    while(true)
    {
        vTaskDelay(500/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        run.change();       //����ָʾ��
        OUT1.change();      //���״̬��ת
        OUT2.change();      //���״̬��ת
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    DEBUG<<"lwIP Initing...\r\n";
    while(lwip_comm_init()!=0)
    {
        DEBUG<<"lwIP Init failed!\r\n";
        delay_ms(1200);
//        my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
        DEBUG<<"Retrying...\r\n";
    }
    DEBUG<<"lwIP Init Successed\r\n";
    //�ȴ�DHCP��ȡ
    DEBUG<<"DHCP IP configing...\r\n";
    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
    {
        lwip_periodic_handle();
    }

    uint8_t speed;
    char buf[30];

    DEBUG<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];

    DEBUG<<"lwIP DHCP Successed\r\n";
    if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
    else sprintf((char*)buf,"Static IP:%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
    DEBUG<<buf;
    speed=LAN8720_Get_Speed();//�õ�����
    if(speed&1<<1)DEBUG<<"Ethernet Speed:100M\r\n";
    else DEBUG<<"Ethernet Speed:10M\r\n";

//    DEBUG<<DNS_get_ip("www.baidu.com");    //DNS����
    MQTT::Subscribe request("v1/devices/me/rpc/request/+",0,1);
    while(true)
    {
        uint8_t num=5;
        uint8_t times=0;
//        {clientId:"daocaoren",userName:"daocaoren",password:"daocaoren"}
        mqtt_demo.Connect(222,74,215,220,31883);
        mqtt_demo.config((string)"daocaoren",(string)"daocaoren",(string)"daocaoren");
        mqtt_demo.SubscribeTopic(request);
        DEBUG<<"linking...\r\n";
        mqtt_demo.Clear();
        while (true){
            delay_ms(100);
            times++;
//            if(times>100){
//                times=0;
////                data_splice(buf,(char*)"����",num++);
//                //����JSON����
//                cJSON *root = cJSON_CreateObject();
//                //��Ӽ�ֵ��
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("����").data(),num++);
//                //��JSON����ת��Ϊ�ַ���
//                sprintf(buf,"%s",cJSON_Print(root));
//                //ɾ��JSON����
//                cJSON_Delete(root);
//                if(num>30)num=5;
//                DEBUG<<buf<<"\r\n";
//                mqtt_demo.PublishData("v1/devices/me/telemetry",buf,0);
//
//            }



            if(mqtt_demo.available()){                              //���յ�����
                mqtt_demo.Message_analyze(mqtt_demo.GetRxbuf());//�������յ�������

                if(mqtt_demo.check_topic(request)){
                    cJSON *root = cJSON_Parse(mqtt_demo.getMsg().data());
                    //���json�Ƿ���ȷcJSON_GetErrorPtr
                    if (root == nullptr) {
                        DEBUG<<"Error before: "<<cJSON_GetErrorPtr()<<"\r\n";
                        break;
                    }
                    cJSON *item = cJSON_GetObjectItem(root,"params");
                    if(item!= nullptr) {
                        if (item->valueint==1) {
                            BEEP.set(ON);
                        } else if (item->valueint==0) {
                            BEEP.set(OFF);
                        }
                    }
                    cJSON_Delete(item);
                    cJSON_Delete(root);
                }
            }

            if(!mqtt_demo.islink())
                break;
        }

    }
}


