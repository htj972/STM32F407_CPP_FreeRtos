#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "RS485.h"
#include "malloc.h"
#include "EC20.h"


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
    };
}led(GPIOE5,TIM6,2);//运行指示灯定时器

_OutPut_ run (GPIOE6);//运行指示灯
_OutPut_ OUT(GPIOB0);            //输出

//Timer_queue tIMS(TIM7,50000);            //定时器队列

_USART_ DEBUG(USART2);             //调试串口
RS485   com(USART3,GPIOB15);
//_USART_ us(USART1);
EC20    ET(USART1);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数
    delay_ms(1000);//延时1s

    my_mem_init(SRAMIN);		//初始化内部内存池
//    ET.init(&us);
    while (!ET.getrdy()){
        delay_ms(1000);
        DEBUG<<".";
    }
    DEBUG<<"EC20:"<<(ET.init()?"OK":"error");
    ET.setdebug(&DEBUG);
    ET.Register(EC20::APN::APN_CMNET);


    ET.mqttopen(1,"222.74.215.220",31883);
    ET.mqttconn(1,"EC20","EC20","EC20");
//    ET.mqttconn(1,"75287960-4549-11ee-9a3c-af6ad99d76ae","123456");


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
        run.change();       //运行指示灯
        OUT.change();      //输出状态反转
    }
}


//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    string da;
    while(true) {
        vTaskDelay(100/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度

        da+=DEBUG.read_data();
        if(da.find("\r\n") != string::npos) {
            ET.mqttpub(1, "v1/devices/me/telemetry", da);
            da.clear();
        }

//        ET<<DEBUG.read_data();
//        DEBUG<<ET.read_data();
    }
}


