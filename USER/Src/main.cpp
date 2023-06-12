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
#include "lwip/inet.h"
#include "lwip/dns.h"



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
#define TASK2_STK_SIZE 		512
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
//        lwip_localtime+=10;
        lwip_setup();
    };
}led(GPIOE5,TIM6,100);//����ָʾ�ƶ�ʱ��

_OutPut_ run (GPIOE6),BEEP (GPIOE4,HIGH);//����ָʾ��
_OutPut_ OUT1(GPIOE0),OUT2(GPIOE1);            //���

//Timer_queue tIMS(TIM7,50000);                 //��ʱ������

_USART_ DEBUG(USART1);                          //���Դ���
_USART_ Lora(USART6);                           //Lora����

Communication SUN(USART3,GPIOB15,TIM7,100);//modbusͨ��

UDP_Class udp_demo(8089);//UDPͨ��

class TCP_Cxlient:public TCP_Client_Class,public Call_Back{
public:
    explicit TCP_Cxlient(uint16_t port):TCP_Client_Class(port){
        this->upload_extern_fun(this);
    }

    void Callback(string str) override{
        this->print(str);
    };
} tcp_demo(8090);//TCPͨ��


int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��

    BEEP.flicker(100,250,2);//��������ʾ

    delay_ms(1000);//��ʱ1s
    SUN.RS485::config(GPIOD8,GPIOD9);//����RS485 GPIO����


    DEBUG<<"lwIP Initing...\r\n";
    uint8_t ret =lwip_comm_init();
    while(ret!=0)
    {
        ret =lwip_comm_init();
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

//{
//"sensor":[{"id":1, "name":"�¶�","value":"20.3","type":"power"},{"id":1, "name":"����","value":"1","type":"value"],
//"inside":[{"id":1, "name":"����","value":"20.3","type":"power"},{"id":1, "name":"�ƹ�","value":"1","type":"value"],
//"outside":[{"id":1, "name":"����","value":"20.3","type":"power"},{"id":1, "name":"����","value":"1","type":"value"],
//}

//task1������
[[noreturn]] void task1_task(void *pvParameters)//alignas(8)
{
    while(true)
    {
        vTaskDelay(200/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        run.change();       //����ָʾ��
        OUT1.change();      //���״̬��ת
        OUT2.change();      //���״̬��ת
        SUN.data_sync();    //modbus����ͬ��
//        DEBUG<<"SUM:"<<(int )SUN.data_BUS.to_u16[0]<<"\r\n";
//        u8 buf[30];
//        sprintf((char*)buf,"{\"id\":1, \"name\":\"����\",\"value\":\"%03.1d\"}",(int )SUN.data_BUS.to_u16[0]);
//        tcp_send_data(buf);    //TCP��������
    }
}

void my_found(const char *name, ip_addr_t *ipaddr, void *arg){
    uint8_t ip[4];
    ip[0] = (ipaddr->addr)>>24;
    ip[1] = (ipaddr->addr)>>16;
    ip[2] = (ipaddr->addr)>>8;
    ip[3] = (ipaddr->addr);
    DEBUG.print("%s ip is :%d.%d.%d.%d.\r\n",name,ip[3], ip[2], ip[1], ip[0]);
}

void vDnsClientTask(){
//    char dfs[]="www.baidu.com";//110.242.68.4
//    DEBUG.print("host ip:%s\r\n", inet_ntoa(dfs));

    ip_addr_t dnsserver;/* Create tcp_ip stack thread */
    IP4_ADDR(&dnsserver,114,114,114,114);/* suozhang,add,2018��1��11��18:03:10 */
    dns_setserver(0, &dnsserver);
    IP4_ADDR(&dnsserver,223,5,5,5);    /* suozhang,add,2018��1��11��18:03:10 */
    dns_setserver(1, &dnsserver);
    dns_init(); /* ��ʼ�� DNS ����2018��1��9��10:56:34 */

    ip_addr_t serverIp;

    dns_gethostbyname("www.baidu.com", &serverIp,my_found,nullptr);
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    udp_demo.bind();    //UDP��
    bool link = false;

    vDnsClientTask();
    while(true)
    {
        delay_ms(2);
        //{"interval":60,"ip":"12.32.54.15","port":1883,"user":"XCSZ","passd":"12345678"}
        if(udp_demo.available()){
            cJSON *root = cJSON_Parse(udp_demo.read_data().data());
            cJSON *interval = cJSON_GetObjectItem(root, "interval");
            cJSON *ip = cJSON_GetObjectItem(root, "ip");
            cJSON *port = cJSON_GetObjectItem(root, "port");
            cJSON *user = cJSON_GetObjectItem(root, "user");
            cJSON *passd = cJSON_GetObjectItem(root, "passd");
            if(interval!= nullptr){
                DEBUG<<"interval:"<<interval->valueint<<"\r\n";
            }
            if(ip!= nullptr){
                DEBUG<<"ip:"<<ip->valuestring<<"\r\n";
                link= true;
            }
            if(port!= nullptr){
                DEBUG<<"port:"<<port->valueint<<"\r\n";
            }
            if(user!= nullptr){
                DEBUG<<"user:"<<user->valuestring<<"\r\n";
            }
            if(passd!= nullptr){
                DEBUG<<"passd:"<<passd->valuestring<<"\r\n";
            }
            cJSON_Delete(root);
        }
        if(link)link=tcp_demo.connect(udp_demo.get_remote_ip());
        while (link){
//            {
//            "sensor":[{"id":1, "name":"�¶�","value":"20.3","type":"value"},{"id":2, "name":"����","value":"1","type":"value"],
//            "inside":[{"id":3, "name":"����","value":"20.3","type":"power"},{"id":4, "name":"�ƹ�","value":"1","type":"value"],
//            "outside":[{"id":5, "name":"����","value":"20.3","type":"power"},{"id":6, "name":"����","value":"1","type":"value"],
//            }
            tcp_demo.print("{\n"
                      "\"sensor\":[{\"id\":1, \"name\":\"�¶�\",\"value\":\"20.3\",\"type\":\"value\"},{\"id\":2, \"name\":\"����\",\"value\":\"%d\",\"type\":\"value\"],\n"
                      "\"inside\":[{\"id\":3, \"name\":\"����\",\"value\":\"1\",\"type\":\"power\"},{\"id\":4, \"name\":\"�ƹ�\",\"value\":\"12.6\",\"type\":\"value\"],\n"
                      "\"outside\":[{\"id\":5, \"name\":\"����\",\"value\":\"1\",\"type\":\"power\"},{\"id\":6, \"name\":\"����\",\"value\":\"1.5\",\"type\":\"value\"],\n"
                      "}",SUN.data_BUS.to_u16[0]);
            delay_ms(1000);
            if(!tcp_demo.islink()){
                link = false;
                break;
            }
            if("unlink"==tcp_demo.read_data()){
                tcp_demo.close();
                link = false;
                break;
            }
        }

    }
}


