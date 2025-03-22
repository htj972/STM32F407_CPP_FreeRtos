#include <regex>
#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "malloc.h"
#include "EC20.h"
#include "ThingsBoard.h"
#include "Kstring.h"
#include "Gateway.h"
#include "Communication.h"
#include "WDG.h"
#include "lwip_comm/lwip_comm.h"
#include "udp/UDP_Class.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		64
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LOGIC_TASK_PRIO		2
//任务堆栈大小
#define LOGIC_STK_SIZE 		128
//任务句柄
TaskHandle_t LOGICTask_Handler;
//任务函数
[[noreturn]] void LOGIC_task(void *pvParameters);

//任务优先级
#define RS485_TASK_PRIO		3
//任务堆栈大小
#define RS485_STK_SIZE 		(128*10)
//任务句柄
TaskHandle_t RS485Task_Handler;
//任务函数
[[noreturn]] void RS485_task(void *pvParameters);
uint16_t tims=0;
//运行指示灯
class T_led_:public _OutPut_,public Call_Back,public Timer{
private:
    bool mode=false;
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void set_mode(bool modex){
        this->mode=modex;
    }
    void Callback(int  ,char** ) override{
        if(mode)this->change();
        Feed_Dog();
        tims++;
    };
}led(GPIOE5,TIM6,2);//运行指示灯定时器

class lwip_:public Timer,public Call_Back{
public:
    lwip_(TIM_TypeDef *TIMx, uint16_t frq) {
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override {
        lwip_setup();
    }
}lwipw(TIM5,100);//运行指示灯定时器

_OutPut_ error_led (GPIOE6);//运行指示灯

//_USART_ DEBUG(USART2);             //调试串口
RS485   com(USART3,GPIOB15,115200);
//Communication COM(USART3,GPIOB15,TIM7,100);
UDP_Class udp_demo(8089);
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    WDG_Init();
    delay_init(168);	//初始化延时函数
    delay_ms(1000);//延时1s

    com.config(GPIOD8,GPIOD9);

    com<<"adsdasd";

    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMCCM);		//初始化内部内存池
    lwip_dhcp_process_handle();

    {
        com<<"lwIP Initing...\r\n";
        while(lwip_comm_init()!=0)
        {
            com<<"lwIP Init failed!\r\n";
            delay_ms(1200);
            com<<"Retrying...\r\n";
        }
        com<<"lwIP Init Successed\r\n";
        //等待DHCP获取
        com<<"DHCP IP configing...\r\n";
        while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
        {
            lwip_periodic_handle();
        }
        uint8_t speed;
        com<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];
        speed=LAN8720_Get_Speed();//得到网速
        if(speed&1<<1)com<<"Ethernet Speed:100M\r\n";
        else com<<"Ethernet Speed:10M\r\n";

    }


//    while(lwip_comm_init()!=0)
//    {
//        delay_ms(1200);
//    }
//
//    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
//    {
//        lwip_periodic_handle();
//    }
    led.set_mode(true);

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
    xTaskCreate((TaskFunction_t )LOGIC_task,
                (const char*    )"LOGIC_task",
                (uint16_t       )LOGIC_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )LOGIC_TASK_PRIO,
                (TaskHandle_t*  )&LOGICTask_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )RS485_task,
                (const char*    )"RS485_task",
                (uint16_t       )RS485_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )RS485_TASK_PRIO,
                (TaskHandle_t*  )&RS485Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
[[noreturn]] void LOGIC_task(void *pvParameters)//alignas(8)
{
    udp_demo.bind();
    while(true)
    {
        delay_ms(2);
        if(udp_demo.available()){
            error_led.change();
            udp_demo.set_romte_ip(udp_demo.get_remote_ip(),8089);
            string cmd=udp_demo.read_data();
            /*{
             * "cmd":{
             * "water":{"mode":"press","value":6.5},
             * "fertilizer":{"mode":"flow","value":0.6},
             * "pail":{"fertilizer":5.2,"volume":500}
             * }
             * }
             */
            //解析json 获取cmd字段



            /*返回 {"data":{"water":{"press":6.3,"flow":80,"quantity":150.3,"PH":6.5,"EC":12.3},
            "fertilizer":{"flow":0.5,"quantity":7.8},"pail":{"fertilizer":0.1,"time":43.2}}}
            */
            udp_demo.write(R"({"data":{"water":{"press":6.3,"flow":80,"quantity":150.3,"PH":6.5,"EC":12.3},"fertilizer":{"flow":0.5,"quantity":7.8},"pail":{"fertilizer":0.1,"time":43.2}}})");
        }
    }
}

//task2任务函数
[[noreturn]] void RS485_task(void *pvParameters)
{
    while(true) {
        delay_ms(1000);


    }
}


