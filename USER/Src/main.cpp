#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "IIC.h"
#include "Communication.h"
#include "MAX31865.h"
#include "DW_DIS.h"
#include "TEOM.h"


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
#define TASK2_STK_SIZE 		1024
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
[[noreturn]] void task2_task(void *pvParameters);



_OutPut_    led(GPIOE6);

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

//TOEM 驱动类
TEOM_Machine teom(GPIOB0,GPIOB1,GPIOD14,TIM4,2);
//通信软件类
Communication m_modebus(USART3,GPIOE15,TIM5,100);
//迪文显示类
DW_DIS MDW(USART6,TIM7,10);




int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数

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

    teom.inital();

    MDW.SetBackLight(20);
    MDW.set_dis_sleep_time(15);
    MDW.Link_TEOM(&teom);
    MDW.init();


//    teom.turn_on();

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


    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    static uint8_t flag=0;
    uint8_t ret=0;
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
        MDW.key_handle();
        MDW.Dis_handle();
    }
}


