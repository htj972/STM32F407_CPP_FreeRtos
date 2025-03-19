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
#define EC20_TASK_PRIO		3
//任务堆栈大小
#define EC20_STK_SIZE 		(128*10)
//任务句柄
TaskHandle_t EC20Task_Handler;
//任务函数
[[noreturn]] void EC20_task(void *pvParameters);
uint16_t tims=0;
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
RS485   com(USART3,GPIOB15);
//Communication COM(USART3,GPIOB15,TIM7,100);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    WDG_Init();
    delay_init(168);	//初始化延时函数
    delay_ms(1000);//延时1s

    com.config(GPIOD8,GPIOD9);

    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMCCM);		//初始化内部内存池
    lwip_dhcp_process_handle();

    while(lwip_comm_init()!=0)
    {
        delay_ms(1200);
    }

    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
    {
        lwip_periodic_handle();
    }


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
    xTaskCreate((TaskFunction_t )EC20_task,
                (const char*    )"EC20_task",
                (uint16_t       )EC20_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )EC20_TASK_PRIO,
                (TaskHandle_t*  )&EC20Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
[[noreturn]] void LOGIC_task(void *pvParameters)//alignas(8)
{
    Kstring da;
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );
    }
}

//task2任务函数
[[noreturn]] void EC20_task(void *pvParameters)
{
    while(true) {
        delay_ms(1000);
    }
}


