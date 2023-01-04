#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "Communication.h"
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
#define TEMP_TASK_PRIO		2
//任务堆栈大小
#define TEMP_STK_SIZE 		128
//任务句柄
TaskHandle_t TEMPTask_Handler;
//任务函数
[[noreturn]] void TEMP_task(void *pvParameters);

//任务优先级
#define PRE_TASK_PRIO		2
//任务堆栈大小
#define PRE_STK_SIZE 		128
//任务句柄
TaskHandle_t PRETask_Handler;
//任务函数
[[noreturn]] void PRE_task(void *pvParameters);

//任务优先级
#define COM_TASK_PRIO		3
//任务堆栈大小
#define COM_STK_SIZE 		512
//任务句柄
TaskHandle_t COMTask_Handler;
//任务函数
[[noreturn]] void COM_task(void *pvParameters);

//任务优先级
#define DIS_TASK_PRIO		3
//任务堆栈大小
#define DIS_STK_SIZE 		1024
//任务句柄
TaskHandle_t DISTask_Handler;
//任务函数
[[noreturn]] void DIS_task(void *pvParameters);


_OutPut_    led(GPIOE6);

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
}led2(GPIOE5,TIM6,2);

//TEOM 驱动类
TEOM_Machine teom(GPIOB0,GPIOB1,GPIOD14,TIM4,2);
//TEOM 温度控制类
TEOM_TEMP TEMP(SPI1,GPIOC0,GPIOC1,GPIOC2,GPIOC3,GPIOA0,TIM1,1000);
//通信软件类
Communication m_modebus(USART3,GPIOE15,TIM5,100);
//迪文显示类
DW_DIS MDW(USART6,TIM7,10);
//差压显示
pressure_dif pressure(GPIOD4,GPIOD5);




int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数
    delay_ms(1000);
    TEMP.initial();
    teom.inital();
    MDW.Link_TEOM(&teom);
    MDW.Link_Temp(&TEMP);
    MDW.Link_Com(&m_modebus);
    MDW.initial();



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
    //创建温度任务
    xTaskCreate((TaskFunction_t )TEMP_task,
                (const char*    )"TEMP_task",
                (uint16_t       )TEMP_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TEMP_TASK_PRIO,
                (TaskHandle_t*  )&TEMPTask_Handler);
    //创建压力任务
    xTaskCreate((TaskFunction_t )PRE_task,
                (const char*    )"PRE_task",
                (uint16_t       )PRE_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )PRE_TASK_PRIO,
                (TaskHandle_t*  )&PRETask_Handler);
    //创建COM任务
    xTaskCreate((TaskFunction_t )COM_task,
                (const char*    )"COM_task",
                (uint16_t       )COM_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )COM_TASK_PRIO,
                (TaskHandle_t*  )&COMTask_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )DIS_task,
                (const char*    )"DIS_task",
                (uint16_t       )DIS_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DIS_TASK_PRIO,
                (TaskHandle_t*  )&DISTask_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//温度读取任务
[[noreturn]] void TEMP_task(void *pvParameters)
{
    while(true) {
        delay_ms(100);
    }
}
//压力读取任务
[[noreturn]] void PRE_task(void *pvParameters)
{
    while(true) {
        delay_ms(100);
        pressure.read();
    }
}
//串通信任务
[[noreturn]] void COM_task(void *pvParameters)
{
    uint8_t sec_t=0;
    while(true) {
        vTaskDelay(200 / portTICK_RATE_MS);
        m_modebus.data_sync();

    }
}
//迪文屏任务
[[noreturn]] void DIS_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
        MDW.key_handle();
        MDW.Dis_handle();
    }
}


