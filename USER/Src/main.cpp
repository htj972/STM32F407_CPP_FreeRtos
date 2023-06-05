#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "Timer_queue.h"
#include "RS485.h"
#include "malloc.h"
#include "lwip_comm/lwip_comm.h"
#include "tcp_client_demo/tcp_client_demo.h"
#include "udp_demo/udp_demo.h"
#include "modbus.h"



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
        lwip_localtime+=10;
    };
}led(GPIOE5,TIM6,100);

_OutPut_ run (GPIOE6),BEEP (GPIOE4,HIGH);
_OutPut_ OUT1(GPIOE0),OUT2(GPIOE1);

Timer_queue tIMS(TIM7,50000);

_USART_ DEBUG(USART1);
_USART_ Lora(USART6);
RS485   YU(USART3,GPIOB15,9600);

modbus SUN(&YU,modbus::HOST);

uint16_t sun_data[2];

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数

    my_mem_init(SRAMIN);		//初始化内部内存池

    BEEP.set(ON);
    delay_ms(100);
    BEEP.set(OFF);
    delay_ms(250);

    BEEP.set(ON);
    delay_ms(100);
    BEEP.set(OFF);

    SUN.config(sun_data,2);

    delay_ms(1000);
    YU.config(GPIOD8,GPIOD9);

//    YU.init();


    DEBUG<<"Ethernet lwIP Test\r\n"<<"KOKIRIKA\r\n"<<"2023-6-2\r\n";

    DEBUG<<"lwIP Initing...\r\n";
    uint8_t ret =lwip_comm_init();
    while(ret!=0)
    {
        ret =lwip_comm_init();
        DEBUG<<"lwIP Init failed!\r\n";
        delay_ms(1200);
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

//task1任务函数
[[noreturn]] void task1_task(void *pvParameters)//alignas(8)
{
    while(true)
    {
        vTaskDelay(200/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
        run.change();
        OUT1.change();
        OUT2.change();
        SUN.modbus_03_send(0,1);
    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );


        udp_demo_test();
        tcp_client_test();


    }
}


