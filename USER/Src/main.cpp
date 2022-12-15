#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "DW_DIS.h"
#include "SPI.h"
#include "MAX31865.h"



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
#define TASK2_STK_SIZE 		128
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
[[noreturn]] void task2_task(void *pvParameters);

_OutPut_    led(GPIOE6);

DW_DIS      MDW(USART6,TIM7,10);
SPI         spi1(SPI1);
MAX31865    temp1(&spi1,GPIOC0);
MAX31865    temp2(&spi1,GPIOC1);
MAX31865    temp3(&spi1,GPIOC2);
MAX31865    temp4(&spi1,GPIOC3);
MAX31865    temp5(&spi1,GPIOA0);

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
}led2(GPIOE5,TIM6,10);



int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);					//初始化延时函数

    temp1.init();
    temp1.config(MAX31865::MODE::PT1000,2000);
    temp2.init();
    temp2.config(MAX31865::MODE::PT1000,2000);
    temp3.init();
    temp3.config(MAX31865::MODE::PT1000,2000);
    temp4.init();
    temp4.config(MAX31865::MODE::PT1000,2000);
    temp5.init();
    temp5.config(MAX31865::MODE::PT1000,2000);
    MDW.Interface_switching(1);
    MDW.SetBackLight(20);
    MDW.set_dis_sleep_time(10*60*15);
    MDW.init();

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
    uint8_t sec_t=0;
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
//        MDW.vspf_Text(0x1000,(char*)"temp:%f",temp1.get_temp());
//        MDW.vspf_Text(0x1040,(char*)"temp:%f",temp2.get_temp());
//        MDW.vspf_Text(0x1080,(char*)"temp:%f",temp3.get_temp());
//        MDW.vspf_Text(0x10C0,(char*)"temp:%f",temp4.get_temp());
//        MDW.vspf_Text(0x1100,(char*)"temp:%f",temp5.get_temp());

    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(200/portTICK_RATE_MS );
        led.change();
        MDW.key_handle();
        MDW.Dis_handle();
    }
}


