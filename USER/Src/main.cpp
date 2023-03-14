#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "TMC220xUart.h"
#include "CD4052.h"
#include "PWM.h"
#include "Timer_queue.h"



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
}led(GPIOE2,TIM6,2);

Timer_queue tIMS(TIM7,50000);

_USART_ DEBUG(USART1);
//_USART_ TMC_U(USART3);

CD4052 TMC_U(GPIOE15,GPIOE14,USART3);
//
//TMC220xUart TMC_X(&TMC_U,GPIOE12,GPIOE13,GPIOA3);

class TMCX:public TMC220xUart{
private:
    uint8_t CD4052_chanel{};
protected:
    CD4052 *uartX{};// 设置串口
public:
    void send_data(TMC220xUart *TMX,uint8_t *str, uint16_t len) override{
        this->uartX->clear();
        this->uartX->write_chanel(CD4052_chanel,str, len);
    };
    TMCX(CD4052 *uartx,uint8_t channel,uint32_t STEP_Pin,uint32_t EN_Pin,uint32_t DIAG_Pin){
        this->init(uartx,STEP_Pin,EN_Pin,DIAG_Pin);
        this->uartX=uartx;
        this->CD4052_chanel=channel;
    };

}TMC_X1(&TMC_U,1,GPIOE12,GPIOE13,GPIOA3),
 TMC_X2(&TMC_U,2,GPIOE10,GPIOE11,GPIOA4),
 TMC_X3(&TMC_U,3,GPIOE8,GPIOE9,GPIOA1),
 TMC_X4(&TMC_U,4,GPIOB1,GPIOE7,GPIOA2);


int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数

    delay_ms(1000);
    TMC_X1.config(64,20,1,100, true);
    TMC_X1.Link_TIMX(&tIMS);
    TMC_X1.Return_to_zero();
    TMC_X1.stallGuard(0);
//    TMC_X2.config(64,10,1,100,true);
//    TMC_X2.Return_to_zero();
//    TMC_X2.stallGuard(0);

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
        vTaskDelay(1000/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
//        TMC_X1.moveTo(1,5);
        TMC_X1.set_site(10);
//        TMC_X2.moveTo(1,5);
        vTaskDelay(1000/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
//        TMC_X1.moveTo(0,5);
        TMC_X1.set_site(5);
//        TMC_X2.moveTo(0,5);
    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );
    }
}


