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



//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小
#define TASK1_STK_SIZE 		512
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
[[noreturn]] void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		2
//任务堆栈大小
#define TASK2_STK_SIZE 		512
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
[[noreturn]] void task2_task(void *pvParameters);

extern u32 lwip_localtime;		//lwip本地时间计数器,单位:ms
//运行指示灯
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
}led(GPIOE5,TIM6,100);//运行指示灯定时器

_OutPut_ run (GPIOE6),BEEP (GPIOE4,HIGH);//运行指示灯
_OutPut_ OUT1(GPIOE0),OUT2(GPIOE1);            //输出

//Timer_queue tIMS(TIM7,50000);                 //定时器队列

_USART_ DEBUG(USART1);                          //调试串口
_USART_ Lora(USART6);                           //Lora串口

Communication SUN(USART3,GPIOB15,TIM7,100);//modbus通信

UDP_Class udp_demo(8089);//UDP通信

class TCP_Cxlient:public TCP_Client_Class,public Call_Back{
public:
    explicit TCP_Cxlient(uint16_t port):TCP_Client_Class(port){
        this->upload_extern_fun(this);
    }

    void Callback(string str) override{
        this->print(str);
    };
} tcp_demo(8090);//TCP通信


int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数

    my_mem_init(SRAMIN);		//初始化内部内存池

    BEEP.flicker(100,250,2);//蜂鸣器提示

    delay_ms(1000);//延时1s
    SUN.RS485::config(GPIOD8,GPIOD9);//配置RS485 GPIO引脚


    DEBUG<<"lwIP Initing...\r\n";
    uint8_t ret =lwip_comm_init();
    while(ret!=0)
    {
        ret =lwip_comm_init();
        DEBUG<<"lwIP Init failed!\r\n";
        delay_ms(1200);
//        my_mem_init(SRAMIN);		//初始化内部内存池
        DEBUG<<"Retrying...\r\n";
    }
    DEBUG<<"lwIP Init Successed\r\n";
    //等待DHCP获取
    DEBUG<<"DHCP IP configing...\r\n";
    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
    {
        lwip_periodic_handle();
    }

    uint8_t speed;
    char buf[30];

    DEBUG<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];

    DEBUG<<"lwIP DHCP Successed\r\n";
    if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
    else sprintf((char*)buf,"Static IP:%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
    DEBUG<<buf;
    speed=LAN8720_Get_Speed();//得到网速
    if(speed&1<<1)DEBUG<<"Ethernet Speed:100M\r\n";
    else DEBUG<<"Ethernet Speed:10M\r\n";




    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,          //任务函数
                (const char*    )"start_task",           //任务名称
                (uint16_t       )START_STK_SIZE,     //任务堆栈大小
                (void*          )nullptr,            //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//{
//"sensor":[{"id":1, "name":"温度","value":"20.3","type":"power"},{"id":1, "name":"光照","value":"1","type":"value"],
//"inside":[{"id":1, "name":"开关","value":"20.3","type":"power"},{"id":1, "name":"灯光","value":"1","type":"value"],
//"outside":[{"id":1, "name":"开关","value":"20.3","type":"power"},{"id":1, "name":"阀门","value":"1","type":"value"],
//}

//task1任务函数
[[noreturn]] void task1_task(void *pvParameters)//alignas(8)
{
    while(true)
    {
        vTaskDelay(200/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
        run.change();       //运行指示灯
        OUT1.change();      //输出状态反转
        OUT2.change();      //输出状态反转
        SUN.data_sync();    //modbus数据同步
//        DEBUG<<"SUM:"<<(int )SUN.data_BUS.to_u16[0]<<"\r\n";
//        u8 buf[30];
//        sprintf((char*)buf,"{\"id\":1, \"name\":\"光照\",\"value\":\"%03.1d\"}",(int )SUN.data_BUS.to_u16[0]);
//        tcp_send_data(buf);    //TCP发送数据
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
    IP4_ADDR(&dnsserver,114,114,114,114);/* suozhang,add,2018年1月11日18:03:10 */
    dns_setserver(0, &dnsserver);
    IP4_ADDR(&dnsserver,223,5,5,5);    /* suozhang,add,2018年1月11日18:03:10 */
    dns_setserver(1, &dnsserver);
    dns_init(); /* 初始化 DNS 服务，2018年1月9日10:56:34 */

    ip_addr_t serverIp;

    dns_gethostbyname("www.baidu.com", &serverIp,my_found,nullptr);
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    udp_demo.bind();    //UDP绑定
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
//            "sensor":[{"id":1, "name":"温度","value":"20.3","type":"value"},{"id":2, "name":"光照","value":"1","type":"value"],
//            "inside":[{"id":3, "name":"开关","value":"20.3","type":"power"},{"id":4, "name":"灯光","value":"1","type":"value"],
//            "outside":[{"id":5, "name":"开关","value":"20.3","type":"power"},{"id":6, "name":"阀门","value":"1","type":"value"],
//            }
            tcp_demo.print("{\n"
                      "\"sensor\":[{\"id\":1, \"name\":\"温度\",\"value\":\"20.3\",\"type\":\"value\"},{\"id\":2, \"name\":\"光照\",\"value\":\"%d\",\"type\":\"value\"],\n"
                      "\"inside\":[{\"id\":3, \"name\":\"开关\",\"value\":\"1\",\"type\":\"power\"},{\"id\":4, \"name\":\"灯光\",\"value\":\"12.6\",\"type\":\"value\"],\n"
                      "\"outside\":[{\"id\":5, \"name\":\"开关\",\"value\":\"1\",\"type\":\"power\"},{\"id\":6, \"name\":\"阀门\",\"value\":\"1.5\",\"type\":\"value\"],\n"
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


